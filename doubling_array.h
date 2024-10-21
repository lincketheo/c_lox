#pragma once

#define DBL_ARR_MAKE_AVAILABLE(type, data, cap)                                \
  static inline void type##_make_available(type *t, size_t newlen) {           \
    type##_ASSERT(t);                                                          \
    while (newlen > t->cap) {                                                  \
      t->data = realloc_or_abort(t->data, sizeof *t->data * t->cap * 2);       \
      t->cap = t->cap * 2;                                                     \
    }                                                                          \
  }
