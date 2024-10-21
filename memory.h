#pragma once

#include "errors.h"
#include <assert.h>
#include <stdlib.h>

/**
 * Linear memory
 * - Does not allow free
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

/**
 * Stack memory
 * - Allows free of only the most recently used block
 */
typedef struct {
  size_t cap;
  size_t len;
  void *top;
  void *data;
} stackmem;

#define stackmem_empty(m) ((m)->cap == 0 && (m)->data == NULL && (m)->len == 0)
#define stackmem_not_empty(m)                                                  \
  ((m)->cap > 0 && (m)->data != NULL && (m)->len >= 0)
#define stackmem_ASSERT(m)                                                     \
  ASSERT(m);                                                                   \
  ASSERT((m)->len <= (m)->cap);                                                \
  ASSERT(stackmem_empty(m) || stackmem_not_empty(m))

stackmem stackmem_create();
void *stackmem_malloc(stackmem *m, size_t len);
void stackmem_free_top(stackmem *m);
void stackmem_freeall(stackmem *m);
