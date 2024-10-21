#include "string.h"
#include "doubling_array.h"
#include "facades.h"
#include <string.h>

#define INITIAL_CAP 10

string string_create() {
  string ret;
  ret.data = malloc_or_abort(INITIAL_CAP * sizeof *ret.data);
  ret.len = 0;
  ret.cap = INITIAL_CAP;
  return ret;
}

void string_free(string s) {
  string_ASSERT(&s); // restrictive free
  free(s.data);
}

static inline void *string_head(string *s) {
  string_ASSERT(s);
  return s->data + s->len;
}

DBL_ARR_MAKE_AVAILABLE(string, data, cap);

void string_append_cstr(string *s, const char *data) {
  string_ASSERT(s);
  ASSERT(data);
  size_t len = strlen(data);
  string_make_available(s, s->len + len);

  ASSERT(s->len + len <= s->cap);
  memcpy(string_head(s), data, len);
  s->len += len;
}

void string_append_cstr_len(string *s, const char *data, size_t dlen) {
  string_ASSERT(s);

  if (dlen == 0)
    return;

  ASSERT(data);
  string_make_available(s, s->len + dlen);

  ASSERT(s->len + dlen <= s->cap);
  memcpy(string_head(s), data, dlen);
  s->len += dlen;
}

void string_append_ch(string *s, const char c) {
  string_ASSERT(s);
  string_make_available(s, s->len + 1);
  s->data[s->len++] = c;
}

char *string_to_cstr(string *s) {
  string_ASSERT(s);
  string_make_available(s, s->len + 1);

  ASSERT(s->len + 1 <= s->cap);
  s->data[s->len] = '\0';
  return s->data;
}

char string_pop_right(string *s) {
  string_ASSERT(s);
  if (s->len == 0) {
    return '\0';
  }
  return s->data[--s->len];
}

void string_reset(string *s) {
  string_ASSERT(s);
  s->len = 0;
}
