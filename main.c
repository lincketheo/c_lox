#include "scanner.h"
#include "string.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

int run(const char *data) { return scanner_run(data); }

int run_file(const char *fname) {
  char *data = fread_malloc(fname);
  run(data);
  free(data);
  return 0;
}

int run_prompt() {
  string s = string_create();
  char *cstr;

  do {
    fprintf(stdout, ">> ");
    stdin_readln(&s);
    cstr = string_to_cstr(&s);
    if (strcmp(cstr, "end") == 0)
      break;
    scanner_run(cstr);
  } while (s.len > 0);

  string_free(s);

  return 0;
}

int main(int argc, char **argv) {
  if (argc > 2) {
    fprintf(stderr, "Usage: %s [file]\n", argv[0]);
    return -1;
  }

  if (argc == 2) {
    return run_file(argv[1]);
  } else {
    return run_prompt();
  }
}
