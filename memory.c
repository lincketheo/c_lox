#include "memory.h"
#include "doubling_array.h"
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

DBL_ARR_MAKE_AVAILABLE(linmem, data, cap);

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

DBL_ARR_MAKE_AVAILABLE(stackmem, data, cap);

stackmem stackmem_create() {
  stackmem ret;
  ret.data = malloc_or_abort(INITIAL_CAP);
  ret.cap = INITIAL_CAP;
  ret.len = 0;
  ret.top = ret.data;
  return ret;
}

void *stackmem_malloc(stackmem *m, size_t len) {
  stackmem_ASSERT(m);
  size_t bytes = len + sizeof len;
  stackmem_make_available(m, m->len + bytes);

  void *head = &((uint8_t *)m->data)[m->len];

  // Write size in bytes of next elem
  size_t prev_block = head - m->top;
  *(size_t *)head = prev_block;
  head += sizeof len;
  m->top = head;

  m->len += bytes;
  ASSERT(head);
  return head;
}

void stackmem_free_top(stackmem *m) {
  stackmem_ASSERT(m);
  ASSERT(m->len > 0);

  void *head = &((uint8_t *)m->data)[m->len];
  ASSERT(head >= m->top);

  size_t prev_size = *(size_t *)(m->top - sizeof prev_size);
  m->len -= ((head - m->top) + sizeof prev_size);
  m->top = m->top - prev_size - sizeof prev_size;
}

void stackmem_freeall(stackmem *m) {
  stackmem_ASSERT(m);
  free(m->data);
  m->cap = 0;
  m->len = 0;
}
