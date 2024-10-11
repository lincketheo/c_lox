#pragma once

#include "string.h"
#include <stdio.h>
#include <stdlib.h>

size_t flen(FILE *fp);

char *fread_malloc(const char *fname);

void stdin_readln(string *str);
