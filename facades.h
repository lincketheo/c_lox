#pragma once

#include "errors.h"
#include "utils.h"
#include <stdio.h>

static inline FILE *fopen_or_abort(const char *fname, const char *mode) {
  FILE *fp = fopen(fname, mode);
  abort_if(fp == NULL, "fopen");
  return fp;
}
static inline void *malloc_or_abort(size_t len) {
  void *ret = malloc(len);
  abort_if(ret == NULL, "malloc");
  return ret;
}
static inline void *realloc_or_abort(void *ptr, size_t newlen) {
  void *ret = realloc(ptr, newlen);
  abort_if(ptr == NULL, "realloc");
  return ret;
}
static inline void fseek_or_abort(FILE *fp, long int offset, int whence) {
  abort_if(fseek(fp, offset, whence), "fseek");
}
static inline size_t ftell_or_abort(FILE *fp) {
  long ret = ftell(fp);
  abort_if(ret < 0, "ftell");
  return (size_t)ret;
}
static inline void fread_or_abort(void *buf, size_t size, size_t n, FILE *fp) {
  abort_if(fread(buf, size, n, fp) != n, "fread");
}
static inline void fclose_or_abort(FILE *fp) { abort_if(fclose(fp), "fclose"); }
static inline char *fgets_or_abort(char *s, int n, FILE *fp) {
  char *ret = fgets(s, n, fp);
  abort_if(ferror(fp), "fgets");
  return ret;
}
