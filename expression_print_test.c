

#include "expression.h"

int main() {
  expr left_e = expr_literal(lt_number(123));
  expr left = expr_unary(ur_minus(&left_e));

  expr right_e = expr_literal(lt_number(45.67));
  expr right = expr_grouping(&right_e);

  expr e = expr_binary(binary_c(&left, OT_PLUS, &right));

  fprintln_expr(stdout, &e);

  return 0;
}
