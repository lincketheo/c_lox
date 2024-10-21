#include "expression.h"
#include "errors.h"

int fprintln_expr_r(FILE *ofp, expr *e) {
  ASSERT(ofp);
  ASSERT(e != NULL);

  switch (e->type) {
  case ET_LITERAL:
    switch (e->l.type) {
    case LT_NIL:
      return fprintf(ofp, "nil");
    case LT_FALSE:
      return fprintf(ofp, "false");
    case LT_TRUE:
      return fprintf(ofp, "true");
    case LT_NUMBER:
      return fprintf(ofp, "%f", e->l.dval);
    case LT_STRING:
      return fprintf(ofp, "%s", e->l.sval);
    }
  case ET_VARIABLE:
    return fprintf(ofp, "%s", e->v);

  case ET_UNARY: {
    int ret = 0;
    switch (e->u.op) {
    case BANG:
      ret = fprintf(ofp, "!");
      break;
    case MINUS:
      ret = fprintf(ofp, "-");
      break;
    default:
      unreachable();
    }
    return ret + fprintln_expr_r(ofp, e->u.e);
  }
  case ET_BINARY: {
    int ret = fprintln_expr_r(ofp, e->b.left);
    switch (e->b.op) {
    case EQUAL_EQUAL:
      ret += fprintf(ofp, " == ");
      break;
    case BANG_EQUAL:
      ret += fprintf(ofp, " != ");
      break;
    case LESS:
      ret += fprintf(ofp, " < ");
      break;
    case LESS_EQUAL:
      ret += fprintf(ofp, " <= ");
      break;
    case GREATER:
      ret += fprintf(ofp, " > ");
      break;
    case GREATER_EQUAL:
      ret += fprintf(ofp, " >= ");
      break;
    case PLUS:
      ret += fprintf(ofp, " + ");
      break;
    case MINUS:
      ret += fprintf(ofp, " - ");
      break;
    case STAR:
      ret += fprintf(ofp, " * ");
      break;
    case SLASH:
      ret += fprintf(ofp, " / ");
      break;
    default:
      unreachable();
    }
    return ret + fprintln_expr_r(ofp, e->b.right);
  }

  case ET_GROUPING: {
    int ret = fprintf(ofp, "(");
    ret += fprintln_expr_r(ofp, e->g);
    return fprintf(ofp, ")");
  }
  }
}

int fprintln_expr(FILE *ofp, expr *e) {
  int ret = fprintln_expr_r(ofp, e);
  return ret + fprintf(ofp, "\n");
}
