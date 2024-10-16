#include "memory.h"
#include "facades.h"
#include <stdint.h>

#define INITIAL_CAP 100000

linmem linmem_create() {
  linmem ret;
  ret.data = malloc_or_abort(INITIAL_CAP);
  ret.cap = INITIAL_CAP;
  ret.len = 0;
  return ret;
}

static inline void linmem_double_capacity(linmem *m) {
  linmem_ASSERT(m);
  m->data = realloc_or_abort(m->data, m->cap * 2);
  m->cap = m->cap * 2;
}

static inline void linmem_make_available(linmem *m, size_t newlen) {
  linmem_ASSERT(m);
  while (newlen > m->cap) {
    linmem_double_capacity(m);
  }
}

void *linmem_malloc(linmem *m, size_t len) {
  linmem_ASSERT(m);
  linmem_make_available(m, m->len + len);
  void *ret = &((uint8_t *)m->data)[m->len];
  m->len += len;
  ASSERT(ret);
  return ret;
}

void linmem_free(linmem *m) {
  linmem_ASSERT(m);
  free(m->data);
  m->cap = 0;
  m->len = 0;
}
