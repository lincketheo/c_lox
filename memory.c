#include "memory.h"
#include "facades.h"
#include <stdint.h>

#define INITIAL_CAP 100000
#define memstack_ASSERT(m)                                                     \
  ASSERT(m);                                                                   \
  ASSERT((m)->len <= (m)->cap);                                                \
  ASSERT((m)->data);                                                           \
  ASSERT((m)->cap > 0)

memstack memstack_create() {
  memstack ret;
  ret.data = malloc_or_abort(INITIAL_CAP);
  ret.cap = INITIAL_CAP;
  ret.len = 0;
  return ret;
}

static inline void memstack_double_capacity(memstack *m) {
  memstack_ASSERT(m);
  m->data = realloc_or_abort(m->data, m->cap * 2);
  m->cap = m->cap * 2;
}

static inline void memstack_make_available(memstack *m, size_t newlen) {
  memstack_ASSERT(m);
  while (newlen > m->cap) {
    memstack_double_capacity(m);
  }
}

void *memstack_malloc(memstack *m, size_t len) {
  memstack_ASSERT(m);
  memstack_make_available(m, m->len + len);
  void *ret = &((uint8_t *)m->data)[m->len];
  m->len += len;
  return ret;
}

void memstack_free(memstack m) {
  memstack_ASSERT(&m);
  free(m.data);
}
