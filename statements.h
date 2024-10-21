#pragma once

#include "expression.h"

typedef enum { ST_EXPR, ST_PRNT, ST_DECL } stmt_t;

typedef struct {
  stmt_t type;
  expr *e;
  char *ident_name;
} stmt;

typedef struct {
  stmt *stmts;
  size_t len;
  size_t cap;
} stmt_arr;

#define stmt_arr_ASSERT(s)                                                     \
  ASSERT(s);                                                                   \
  ASSERT(s->stmts);                                                            \
  ASSERT(s->cap > 0);

stmt_arr stmt_arr_create();

void stmt_arr_free(stmt_arr *s);

void stmt_arr_push(stmt_arr *s, stmt st);
