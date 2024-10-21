#include "utils.h"
#include "errors.h"
#include "facades.h"
#include "string.h"

#include <stdio.h>
#include <string.h>

size_t flen(FILE *fp) {
  ASSERT(fp);

  // Get the starting position
  long start = ftell_or_abort(fp);

  // Seek to the end
  fseek_or_abort(fp, 0, SEEK_END);

  // Get offset
  size_t end = ftell_or_abort(fp);

  // Reset
  fseek_or_abort(fp, start, SEEK_SET);

  return end;
}

char *fread_malloc(const char *fname) {
  ASSERT(fname);

  // Open file
  FILE *fp = fopen_or_abort(fname, "rb");

  // Allocate return buffer
  size_t fsize = flen(fp);
  char *ret = malloc_or_abort((fsize + 1) * sizeof *ret);
  ret[fsize] = '\0';

  // Implicitly assumes ftell(fp) == 0
  fread_or_abort(ret, sizeof *ret, fsize, fp);

  // Clean up
  fclose_or_abort(fp);

  return ret;
}

void stdin_readln(string *str) {
  string_ASSERT(str);

  string_reset(str);
  char buffer[6];
  while (fgets_or_abort(buffer, sizeof buffer, stdin) != NULL) {
    string_append_cstr(str, buffer);
    if (strchr(buffer, '\n'))
      break;
  }

  // Remove the newline
  if (!feof(stdin)) {
    char nl = string_pop_right(str);
    ASSERT(nl == '\n');
  }
}
