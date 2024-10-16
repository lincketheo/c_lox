#pragma once

#include <errno.h>

void fatal_error(const char *format, ...) __attribute__((noreturn));
void compile_error(const int line, const char *format, ...);
void runtime_error(const char *format, ...);

#define abort_if(expr, context)                                                \
  do {                                                                         \
    if (expr) {                                                                \
      if (errno)                                                               \
        perror(context);                                                       \
      fatal_error("context: %s" #expr ". Aborting.", context);                 \
    }                                                                          \
  } while (0)

// A Bug
#define ASSERT(expr)                                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fatal_error("BUG! ASSERTION: " #expr " failed. "                         \
                  "Location: (%s):%s:%d\n",                                    \
                  __FILE__, __func__, __LINE__);                               \
    }                                                                          \
  } while (0)

#define TODO(msg)                                                              \
  do {                                                                         \
    fatal_error("TODO! " msg " "                                               \
                "Location: (%s):%s:%d\n",                                      \
                __FILE__, __func__, __LINE__);                                 \
  } while (0)

#define unreachable()                                                          \
  fatal_error("BUG! Unreachable! (%s):%s:%d\n", __FILE__, __func__, __LINE__);
