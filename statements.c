#include "statements.h"
#include "doubling_array.h"
#include "facades.h"

#define INITIAL_CAP 10

stmt_arr stmt_arr_create() {
  stmt_arr ret;
  ret.stmts = malloc_or_abort(INITIAL_CAP * sizeof *ret.stmts);
  ret.len = 0;
  ret.cap = INITIAL_CAP;
  return ret;
}

void stmt_arr_free(stmt_arr *s) {
  stmt_arr_ASSERT(s);
  free(s->stmts);
  s->cap = 0;
  s->len = 0;
}

DBL_ARR_MAKE_AVAILABLE(stmt_arr, stmts, cap);

void stmt_arr_push(stmt_arr *s, stmt st) {
  stmt_arr_ASSERT(s);
  stmt_arr_make_available(s, s->len + 1);
  s->stmts[s->len++] = st;
}
