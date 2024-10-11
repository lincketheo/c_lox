#pragma once

#include <stdlib.h>

/**
 * Memory bank. Use this when:
 * 1. You don't need reallocs
 */
typedef struct {
  size_t cap;
  size_t len;
  void *data;
} memstack;

memstack memstack_create();
void *memstack_malloc(memstack *m, size_t len);
void memstack_free(memstack m);
