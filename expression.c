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

  case ET_UNARY: {
    int ret = 0;
    switch (e->u.type) {
    case UT_BANG:
      ret = fprintf(ofp, "!");
      break;
    case UT_MINUS:
      ret = fprintf(ofp, "-");
      break;
    }
    return ret + fprintln_expr_r(ofp, e->u.e);
  }
  case ET_BINARY: {
    int ret = fprintln_expr_r(ofp, e->b.left);
    switch (e->b.op) {
    case OT_EQUAL_EQUAL:
      ret += fprintf(ofp, " == ");
      break;
    case OT_BANG_EQUAL:
      ret += fprintf(ofp, " != ");
      break;
    case OT_LESS:
      ret += fprintf(ofp, " < ");
      break;
    case OT_LESS_EQUAL:
      ret += fprintf(ofp, " <= ");
      break;
    case OT_GREATER:
      ret += fprintf(ofp, " > ");
      break;
    case OT_GREATER_EQUAL:
      ret += fprintf(ofp, " >= ");
      break;
    case OT_PLUS:
      ret += fprintf(ofp, " + ");
      break;
    case OT_MINUS:
      ret += fprintf(ofp, " - ");
      break;
    case OT_STAR:
      ret += fprintf(ofp, " * ");
      break;
    case OT_SLASH:
      ret += fprintf(ofp, " / ");
      break;
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
