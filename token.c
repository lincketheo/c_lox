#include "token.h"
#include "facades.h"
#include "memory.h"
#include "string.h"
#include <string.h>

#define INITIAL_CAP 10

#define token_ASSERT(t) ASSERT(t);

#define __tttostr_case(t)                                                      \
  case t:                                                                      \
    return #t;

const char *tttostr(token_t t) {
  switch (t) { ttforeach(__tttostr_case); }
  return NULL;
}

token token_create(memstack *m, const char *loc, size_t len, token_t type,
                   int line) {
  token ret;
  ret.literal = memstack_malloc(m, len + 1);
  memcpy(ret.literal, loc, len);
  ret.literal[len] = '\0';
  ret.type = type;
  ret.line = line;
  return ret;
}

token_arr token_arr_create() {
  token_arr ret;
  ret.mem = memstack_create();
  ret.tokens = malloc_or_abort(INITIAL_CAP * sizeof *ret.tokens);
  ret.len = 0;
  ret.cap = INITIAL_CAP;
  return ret;
}

void token_arr_free(token_arr arr) {
  token_arr_ASSERT(&arr);
  memstack_free(arr.mem);
  free(arr.tokens);
}

static inline void token_arr_double_capacity(token_arr *t) {
  token_arr_ASSERT(t);
  t->tokens = realloc_or_abort(t->tokens, t->cap * 2);
  t->cap = t->cap * 2;
}

static inline void token_arr_make_available(token_arr *t, size_t newlen) {
  token_arr_ASSERT(t);
  while (newlen > t->cap) {
    token_arr_double_capacity(t);
  }
}

token token_arr_push(token_arr *t, const char *loc, size_t len, token_t type,
                     int line) {
  token_arr_ASSERT(t);
  token_arr_make_available(t, t->len + 1);
  token tk = token_create(&t->mem, loc, len, type, line);
  t->tokens[t->len++] = tk;
  return tk;
}
