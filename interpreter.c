
#include "interpreter.h"
#include "errors.h"
#include "expression.h"
#include "memory.h"
#include "statements.h"
#include "token.h"
#include "value.h"
#include "var_env.h"

static int interpret_expr(linmem *mem, expr *e, value *i, var_env *env);

static void interpret_literal(linmem *mem, literal b, value *i) {
  ASSERT(i);

  switch (b.type) {
  case LT_NIL:
    i->type = V_NIL;
    break;
  case LT_TRUE:
    i->type = V_BOOL;
    i->bool_val = 1;
    break;
  case LT_FALSE:
    i->type = V_BOOL;
    i->bool_val = 0;
    break;
  case LT_NUMBER:
    i->type = V_NUMBER;
    i->dval = b.dval;
    break;
  case LT_STRING:
    ASSERT(b.sval);
    i->type = V_STRING;
    i->sval = b.sval;
    break;
  default:
    unreachable();
  }
}

static int interpret_unary(linmem *mem, unary b, value *i, var_env *env) {
  ASSERT(i);
  if (interpret_expr(mem, b.e, i, env))
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

static int interpret_binary(linmem *mem, binary b, value *i, var_env *env) {
  value left;
  value right;

  if (interpret_expr(mem, b.left, &left, env))
    return -1;
  if (interpret_expr(mem, b.right, &right, env))
    return -1;

  switch (b.op) {
  case EQUAL_EQUAL:
    i->bool_val = equal_equal(&left, &right);
    i->type = V_BOOL;
    return 0;
  case BANG_EQUAL:
    i->bool_val = !equal_equal(&left, &right);
    i->type = V_BOOL;
    return 0;
  case LESS: {
    int val = less(&left, &right);
    if (val < 0)
      return -1;
    i->type = V_BOOL;
    i->bool_val = val;
    return 0;
  }
  case LESS_EQUAL: {
    int val = greater(&left, &right);
    if (val < 0)
      return -1;
    i->type = V_BOOL;
    i->bool_val = !val;
    return 0;
  }
  case GREATER: {
    int val = greater(&left, &right);
    if (val < 0)
      return -1;
    i->type = V_BOOL;
    i->bool_val = val;
    return 0;
  }
  case GREATER_EQUAL: {
    int val = less(&left, &right);
    if (val < 0)
      return -1;
    i->type = V_BOOL;
    i->bool_val = !val;
    return 0;
  }
  case PLUS: {
    if (plus(mem, &left, &right))
      return -1;
    *i = left;
    return 0;
  }
  case MINUS: {
    if (minus(&left, &right))
      return -1;
    *i = left;
    return 0;
  }
  case STAR: {
    if (star(&left, &right))
      return -1;
    *i = left;
    return 0;
  }
  case SLASH: {
    if (slash(&left, &right))
      return -1;
    *i = left;
    return 0;
  }
  default:
    unreachable();
  }
}

static int interpret_variable(linmem *mem, char *name, value *i, var_env *env) {
  value *ret = var_env_get(env, name);
  if (ret == NULL)
    return -1;
  *i = *ret;
  return 0;
}

static int interpret_expr(linmem *mem, expr *e, value *i, var_env *env) {
  ASSERT(e);

  switch (e->type) {
  case ET_LITERAL:
    interpret_literal(mem, e->l, i);
    return 0;
  case ET_UNARY:
    return interpret_unary(mem, e->u, i, env);
  case ET_BINARY:
    return interpret_binary(mem, e->b, i, env);
  case ET_GROUPING:
    return interpret_expr(mem, e->g, i, env);
  case ET_VARIABLE:
    return interpret_variable(mem, e->v, i, env);
  default:
    unreachable();
  }
}

static inline int interpret_expr_stmt(linmem *mem, stmt *s, var_env *env) {
  ASSERT(s);
  ASSERT(mem);
  value i;
  return interpret_expr(mem, s->e, &i, env);
}

static inline int interpret_print_stmt(linmem *mem, stmt *s, var_env *env) {
  ASSERT(s);
  ASSERT(mem);
  value i;
  if (interpret_expr(mem, s->e, &i, env))
    return -1;
  value_println(stdout, i);
  return 0;
}

static inline int interpret_decl_stmt(linmem *mem, stmt *s, var_env *env) {
  ASSERT(s);
  ASSERT(mem);
  value i;
  if (interpret_expr(mem, s->e, &i, env))
    return -1;
  var_env_define(env, s->ident_name, i);
  return 0;
}

static inline int interpret_stmt(linmem *mem, stmt *s, var_env *env) {
  switch (s->type) {
  case ST_EXPR:
    return interpret_expr_stmt(mem, s, env);
  case ST_PRNT:
    return interpret_print_stmt(mem, s, env);
  case ST_DECL:
    return interpret_decl_stmt(mem, s, env);
  }
}

int interpret_stmts(linmem *mem, stmt_arr *s, var_env *env) {
  stmt_arr_ASSERT(s);
  ASSERT(mem);
  int ret = 0;
  for (int i = 0; i < s->len; ++i) {
    if (interpret_stmt(mem, &s->stmts[i], env))
      ret = -1;
  }
  return ret;
}
