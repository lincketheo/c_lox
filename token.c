#include "token.h"
#include "doubling_array.h"
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

token token_create(linmem *m, const char *loc, size_t len, token_t type,
                   int line) {
  token ret = {.type = type, .line = line};

  ret.literal = linmem_malloc(m, len + 1);
  memcpy(ret.literal, loc, len);
  ret.literal[len] = '\0';

  if (type == STRING) {
    // str without quotes
    ret.str_val = linmem_malloc(m, len + 1 - 2);
    memcpy(ret.str_val, &loc[1], len - 2);
    ret.str_val[len - 1] = '\0';
  }
  if (type == NUMBER) {
    ret.n_val = atof(ret.literal);
  }

  return ret;
}

void token_arr_print(token_arr *arr) {
  token_arr_ASSERT(arr);

  for (int i = 0; i < arr->len; ++i) {
    token t = arr->tokens[i];
    if (t.type == STRING) {
      printf("%s %s %s\n", tttostr(t.type), t.literal, t.str_val);
    } else if (t.type == NUMBER) {
      printf("%s %s %f\n", tttostr(t.type), t.literal, t.n_val);
    } else {
      printf("%s %s\n", tttostr(t.type), t.literal);
    }
  }
}

token_arr token_arr_create() {
  token_arr ret;
  ret.mem = linmem_create();
  ret.tokens = malloc_or_abort(INITIAL_CAP * sizeof *ret.tokens);
  ret.len = 0;
  ret.cap = INITIAL_CAP;
  return ret;
}

void token_arr_free(token_arr *arr) {
  token_arr_ASSERT(arr);
  linmem_free(&arr->mem);
  free(arr->tokens);
  arr->len = 0;
  arr->cap = 0;
}

DBL_ARR_MAKE_AVAILABLE(token_arr, tokens, cap);

void token_arr_push(token_arr *t, const char *loc, size_t len, token_t type,
                    int line) {
  token_arr_ASSERT(t);
  token_arr_make_available(t, t->len + 1);
  t->tokens[t->len++] = token_create(&t->mem, loc, len, type, line);
}
