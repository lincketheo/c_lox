#pragma once

#include "errors.h"
#include <stdlib.h>

typedef struct {
  char *data;
  size_t len;
  size_t cap;
} string;

#define string_ASSERT(s)                                                       \
  ASSERT(s);                                                                   \
  ASSERT((s)->len <= (s)->cap);                                                \
  ASSERT((s)->data);                                                           \
  ASSERT((s)->cap > 0)

string string_create();

void string_free(string s);

void string_append_cstr(string *s, const char *data);

void string_append_cstr_len(string *s, const char *data, size_t dlen);

void string_append_ch(string *s, const char c);

char *string_to_cstr(string *s);

char string_pop_right(string *s);

void string_reset(string *s);
