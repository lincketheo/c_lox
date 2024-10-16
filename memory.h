#pragma once

#include "errors.h"
#include <stdlib.h>

/**
 * Memory bank. Use this when:
 * 1. You don't need reallocs
 */
typedef struct {
  size_t cap;
  size_t len;
  void *data;
} linmem;

#define linmem_empty(m) ((m)->cap == 0 && (m)->data == NULL && (m)->len == 0)
#define linmem_not_empty(m) ((m)->cap > 0 && (m)->data != NULL && (m)->len >= 0)
#define linmem_ASSERT(m)                                                       \
  ASSERT(m);                                                                   \
  ASSERT((m)->len <= (m)->cap);                                                \
  ASSERT(linmem_empty(m) || linmem_not_empty(m))

linmem linmem_create();
void *linmem_malloc(linmem *m, size_t len);
void linmem_free(linmem *m);
