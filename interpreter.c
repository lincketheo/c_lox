
#include "errors.h"
#include "expression.h"
#include "token.h"
#include <stdarg.h>
#include <string.h>

typedef enum { IT_STRING, IT_NUMBER, IT_NIL, IT_BOOL } interp_t;

#define __itttostr_case(t)                                                     \
  case t:                                                                      \
    return #t

static const char *itttostr(interp_t i) {
  switch (i) {
    __itttostr_case(IT_STRING);
    __itttostr_case(IT_NUMBER);
    __itttostr_case(IT_NIL);
    __itttostr_case(IT_BOOL);
  }
}

typedef struct {
  union {
    char *sval;
    double dval;
    int bool_val;
  };
  interp_t type;
} interp;

static int interpret(expr *e, interp *i);

static void interpret_literal(literal b, interp *i) {
  ASSERT(i);

  switch (b.type) {
  case LT_NIL:
    i->type = IT_NIL;
    break;
  case LT_TRUE:
    i->type = IT_BOOL;
    i->bool_val = 1;
    break;
  case LT_FALSE:
    i->type = IT_BOOL;
    i->bool_val = 0;
    break;
  case LT_NUMBER:
    i->type = IT_NUMBER;
    i->dval = b.dval;
    break;
  case LT_STRING:
    ASSERT(b.sval);
    i->type = IT_STRING;
    i->sval = b.sval;
    break;
  default:
    unreachable();
  }
}

static inline int number_cast(interp *i) {
  ASSERT(i);

  switch (i->type) {
  case IT_NUMBER:
    return 0;
  case IT_BOOL:
    i->type = IT_NUMBER;
    i->dval = (i->bool_val ? 1 : 0);
    return 0;
  case IT_NIL:
    runtime_error("Cannot cast NIL to a number");
    return -1;
  case IT_STRING:
    ASSERT(i->sval);
    runtime_error("Cannot cast a String: %s to a number", i->sval);
    return -1;
  default:
    unreachable();
  }
}

static inline void bool_cast(interp *i) {
  switch (i->type) {
  case IT_NUMBER:
    i->type = IT_BOOL;
    i->bool_val = i->dval == 0 ? 0 : 1;
    break;
  case IT_BOOL:
    break;
  case IT_NIL:
    i->type = IT_BOOL;
    i->bool_val = 0;
    break;
  case IT_STRING:
    ASSERT(i->sval);
    i->type = IT_BOOL;
    i->bool_val = i->sval[0] != '\0';
    break;
  default:
    unreachable();
  }
}

static inline void minus_number(interp *i) {
  ASSERT(i->type == IT_NUMBER);
  i->dval = -i->dval;
}

static inline void bang_bool(interp *i) {
  ASSERT(i->type == IT_BOOL);
  i->bool_val = !i->bool_val;
}

static int interpret_unary(unary b, interp *i) {
  ASSERT(i);
  if (interpret(b.e, i))
    return -1;

  switch (b.op) {
  case MINUS: {
    if (number_cast(i))
      return -1;
    minus_number(i);
    return 0;
  }
  case BANG: {
    bool_cast(i);
    bang_bool(i);
    return 0;
  }
  default:
    unreachable();
  }
}

static int equal_equal(interp *left, interp *right) {
  ASSERT(left);
  ASSERT(right);

  if (left->type == NIL && right->type == NIL)
    return 1;
  if (left->type == NIL)
    return 0;

  if (left->type == STRING && right->type == STRING)
    return strcmp(left->sval, right->sval) == 0;
  if (left->type == STRING)
    return 0;

  ASSERT(left->type != STRING);
  ASSERT(left->type != NIL);

  switch (left->type) {
  case IT_BOOL:
    bool_cast(right);
    return left->bool_val == right->bool_val;
  case IT_NUMBER:
    number_cast(right);
    return left->dval == right->dval;
  default:
    unreachable();
  }
}

static int str_less(const char *left, const char *right) {
  ASSERT(left);
  ASSERT(right);
  int i = 0;
  for (; left[i] != '\0' && right[i] != '\0'; ++i) {
    if (left[i] != right[i])
      return left[i] < right[i];
  }
  return left[i] < right[i];
}

static int str_greater(const char *left, const char *right) {
  ASSERT(left);
  ASSERT(right);
  int i = 0;
  for (; left[i] != '\0' && right[i] != '\0'; ++i) {
    if (left[i] != right[i])
      return left[i] > right[i];
  }
  return left[i] > right[i];
}

static int check_binary_types(interp *left, interp *right, char *op) {
  ASSERT(left);
  ASSERT(right);

  if (left->type == IT_NIL || right->type == IT_NIL) {
    runtime_error("Unsupported operation %s for NIL\n", op);
    return -1;
  }

  if ((left->type == STRING && right->type != STRING) ||
      (left->type != STRING && right->type == STRING)) {
    runtime_error("Unsupported operation %s for STRING and non STRING type",
                  op);
    return -1;
  }

  return 0;
}

static int less(interp *left, interp *right, int *dest) {
  if (check_binary_types(left, right, "<"))
    return -1;

  if (left->type == STRING && right->type == STRING) {
    *dest = str_less(left->sval, right->sval);
    return 0;
  }

  ASSERT(left->type != STRING);
  ASSERT(right->type != STRING);
  ASSERT(left->type != NIL);
  ASSERT(right->type != NIL);

  if (number_cast(left))
    return -1;
  if (number_cast(right))
    return -1;

  *dest = left->dval < right->dval;

  return 0;
}

static int greater(interp *left, interp *right, int *dest) {
  if (check_binary_types(left, right, ">"))
    return -1;

  if (left->type == STRING && right->type == STRING) {
    *dest = str_greater(left->sval, right->sval);
    return 0;
  }

  ASSERT(left->type != STRING);
  ASSERT(right->type != STRING);
  ASSERT(left->type != NIL);
  ASSERT(right->type != NIL);

  if (number_cast(left))
    return -1;
  if (number_cast(right))
    return -1;

  *dest = left->dval > right->dval;

  return 0;
}

static int plus(interp *left, interp *right, int *dest) {
  if (check_binary_types(left, right, "-"))
    return -1;

  if (left->type == STRING && right->type == STRING) {
    TODO("Add strings");
    unreachable();
  }

  ASSERT(left->type != STRING);
  ASSERT(right->type != STRING);
  ASSERT(left->type != NIL);
  ASSERT(right->type != NIL);

  if (number_cast(left))
    return -1;
  if (number_cast(right))
    return -1;

  *dest = left->dval + right->dval;

  return 0;
}

static int minus(interp *left, interp *right, int *dest) {
  if (check_binary_types(left, right, "-"))
    return -1;

  if (left->type == STRING && right->type == STRING) {
    TODO("Subtract strings");
    unreachable();
  }

  ASSERT(left->type != STRING);
  ASSERT(right->type != STRING);
  ASSERT(left->type != NIL);
  ASSERT(right->type != NIL);

  if (number_cast(left))
    return -1;
  if (number_cast(right))
    return -1;

  *dest = left->dval + right->dval;

  return 0;
}

static int binary_dont_allow(interp *left, interp *right, char *op, int pairs,
                             ...) {
  ASSERT(left);
  ASSERT(right);
  ASSERT(op);
  ASSERT(pairs > 0);

  va_list args;
  va_start(args, pairs);

  for (int i = 0; i < pairs; ++i) {
    if (left->type == va_arg(args, interp_t) &&
        right->type == va_arg(args, interp_t)) {
      runtime_error("Unsupported operation %s for operands %s %s\n", op,
                    itttostr(left->type), itttostr(right->type));
      return -1;
    }
  }

  ASSERT(left);
  ASSERT(right);

  if (left->type == IT_NIL || right->type == IT_NIL) {
  }

  if ((left->type == STRING && right->type != STRING) ||
      (left->type != STRING && right->type == STRING)) {
    runtime_error("Unsupported operation %s for STRING and non STRING type",
                  op);
    return -1;
  }

  return 0;
  return 0;
}

static int star(interp *left, interp *right, int *dest) {
  if (check_binary_types(left, right, "*"))
    return -1;

  if (left->type == STRING && right->type == STRING) {
    TODO("Add strings");
    unreachable();
  }

  if (number_cast(left))
    return -1;
  if (number_cast(right))
    return -1;

  *dest = left->dval + right->dval;

  return 0;
}

static int slash(interp *left, interp *right, int *dest) {
  if (check_binary_types(left, right, "/"))
    return -1;

  if (left->type == STRING && right->type == STRING) {
    TODO("Add strings");
    unreachable();
  }

  ASSERT(left->type != STRING);
  ASSERT(right->type != STRING);
  ASSERT(left->type != NIL);
  ASSERT(right->type != NIL);

  if (number_cast(left))
    return -1;
  if (number_cast(right))
    return -1;

  *dest = left->dval + right->dval;

  return 0;
}

static int interpret_binary(binary b, interp *i) {
  interp left;
  if (interpret(b.left, &left))
    return -1;
  if (interpret(b.right, i))
    return -1;

  switch (b.op) {
  case EQUAL_EQUAL:
    i->bool_val = equal_equal(&left, i);
    i->type = IT_BOOL;
    return 0;
  case BANG_EQUAL:
    i->bool_val = !equal_equal(&left, i);
    i->type = IT_BOOL;
    return 0;
  case LESS: {
    int val;
    if (less(&left, i, &val))
      return -1;
    i->type = IT_BOOL;
    i->bool_val = val;
    return 0;
  }
  case LESS_EQUAL: {
    int val;
    if (less(&left, i, &val))
      return -1;
    i->type = IT_BOOL;
    i->bool_val = !val;
    return 0;
  }
  case GREATER: {
    int val;
    if (less(&left, i, &val))
      return -1;
    i->type = IT_BOOL;
    i->bool_val = val;
    return 0;
  }
  case GREATER_EQUAL: {
    int val;
    if (less(&left, i, &val))
      return -1;
    i->type = IT_BOOL;
    i->bool_val = val;
    return 0;
  }
  case PLUS:
  case MINUS:
  case STAR:
  case SLASH:
  default:
    unreachable();
  }
}

static int interpret(expr *e, interp *i) {
  ASSERT(e);

  switch (e->type) {
  case ET_LITERAL:
    interpret_literal(e->l, i);
    return 0;
  case ET_UNARY:
    return interpret_unary(e->u, i);
  case ET_BINARY:
    return interpret_binary(e->b, i);
  case ET_GROUPING:
    return interpret(e->g, i);
  default:
    unreachable();
  }
}
