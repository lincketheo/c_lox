#pragma once

#include "string.h"
#include <stdio.h>
#include <stdlib.h>

size_t flen(FILE *fp);

char *fread_malloc(const char *fname);

void stdin_readln(string *str);

int str_less(const char *left, const char *right);

int str_greater(const char *left, const char *right);
