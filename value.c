#include "value.h"
#include "errors.h"
#include <string.h>

int number_cast(value *i) {
  ASSERT(i);

  switch (i->type) {
  case V_NUMBER:
    return 0;
  case V_BOOL:
    i->type = V_NUMBER;
    i->dval = (i->bool_val ? 1 : 0);
    return 0;
  case V_NIL:
    runtime_error("Cannot cast NIL to a number\n");
    return -1;
  case V_STRING:
    ASSERT(i->sval);
    runtime_error("Cannot cast a String: \"%s\" to a number\n", i->sval);
    return -1;
  default:
    unreachable();
  }
}

void bool_cast(value *i) {
  switch (i->type) {
  case V_NUMBER:
    i->type = V_BOOL;
    i->bool_val = i->dval == 0 ? 0 : 1;
    break;
  case V_BOOL:
    break;
  case V_NIL:
    i->type = V_BOOL;
    i->bool_val = 0;
    break;
  case V_STRING:
    ASSERT(i->sval);
    i->type = V_BOOL;
    i->bool_val = i->sval[0] != '\0';
    break;
  default:
    unreachable();
  }
}

void minus_number(value *i) {
  ASSERT(i->type == V_NUMBER);
  i->dval = -i->dval;
}

void bang_bool(value *i) {
  ASSERT(i->type == V_BOOL);
  i->bool_val = !i->bool_val;
}

/**
 * @brief Checks if left == right
 * modifies left and right
 * @return 1 or 0 if they are equal or not. Doesn't fail
 */
int equal_equal(value *left, value *right) {
  ASSERT(left);
  ASSERT(right);

  if (left->type == V_NIL && right->type == V_NIL)
    return 1;
  if (left->type == V_NIL)
    return 0;

  if (left->type == V_STRING && right->type == V_STRING)
    return strcmp(left->sval, right->sval) == 0;
  if (left->type == V_STRING)
    return 0;

  switch (left->type) {
  case V_BOOL:
    bool_cast(right);
    return left->bool_val == right->bool_val;
  case V_NUMBER:
    number_cast(right);
    return left->dval == right->dval;
  default:
    unreachable();
  }
}

static char *str_plus(linmem *mem, const char *left, const char *right) {
  size_t llen = strlen(left);
  size_t rlen = strlen(right);
  char *ret = linmem_malloc(mem, llen + rlen + 1);
  memcpy(ret, left, llen);
  memcpy(ret + llen, right, rlen);
  ret[llen + rlen] = '\0';
  return ret;
}

int plus(linmem *mem, value *dest, value *right) {
  if (dest->type == V_STRING && right->type == V_STRING) {
    dest->sval = str_plus(mem, dest->sval, right->sval);
    return 0;
  }

  if (number_cast(dest))
    return -1;
  if (number_cast(right))
    return -1;

  dest->dval += right->dval;

  return 0;
}

int minus(value *dest, value *right) {
  if (number_cast(dest))
    return -1;
  if (number_cast(right))
    return -1;

  dest->dval -= right->dval;
  return 0;
}

int star(value *dest, value *right) {
  if (number_cast(dest))
    return -1;
  if (number_cast(right))
    return -1;

  dest->dval *= right->dval;
  return 0;
}

int slash(value *dest, value *right) {
  if (number_cast(dest))
    return -1;
  if (number_cast(right))
    return -1;

  dest->dval /= right->dval;
  return 0;
}

int str_less(const char *left, const char *right) {
  ASSERT(left);
  ASSERT(right);
  int i = 0;
  for (; left[i] != '\0' && right[i] != '\0'; ++i) {
    if (left[i] != right[i])
      return left[i] < right[i];
  }
  return left[i] < right[i];
}

int str_greater(const char *left, const char *right) {
  ASSERT(left);
  ASSERT(right);
  int i = 0;
  for (; left[i] != '\0' && right[i] != '\0'; ++i) {
    if (left[i] != right[i])
      return left[i] > right[i];
  }
  return left[i] > right[i];
}

/**
 * @brief Returns if left is less than right (type agnostic)
 *
 * @return 0 or 1 for result, -1 if runtime error
 */
int less(value *left, value *right) {
  if (left->type == V_STRING && right->type == V_STRING)
    return str_less(left->sval, right->sval);

  if (number_cast(left))
    return -1;
  if (number_cast(right))
    return -1;

  return left->dval < right->dval;
}

int greater(value *left, value *right) {
  if (left->type == V_STRING && right->type == V_STRING)
    return str_greater(left->sval, right->sval);

  if (number_cast(left))
    return -1;
  if (number_cast(right))
    return -1;

  return left->dval > right->dval;
}

void value_println(FILE *ofp, value i) {
  switch (i.type) {
  case V_STRING:
    fprintf(ofp, "%s\n", i.sval);
    break;
  case V_NUMBER:
    fprintf(ofp, "%f\n", i.dval);
    break;
  case V_BOOL:
    fprintf(ofp, "%s\n", i.bool_val ? "TRUE" : "FALSE");
    break;
  case V_NIL:
    fprintf(ofp, "NIL\n");
    break;
  }
}
