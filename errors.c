
#include "errors.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void fatal_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  abort();
}

void compile_error(const int line, const char *format, ...) {
  va_list args;
  va_start(args, format);
  fprintf(stderr, "[line %d] Error: ", line);
  vfprintf(stderr, format, args);
  va_end(args);
}

void runtime_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, format, args);
  va_end(args);
}
