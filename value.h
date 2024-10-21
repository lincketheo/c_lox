#pragma once

#include "memory.h"
#include <stdio.h>

typedef enum {
  V_STRING,
  V_NUMBER,
  V_NIL,
  V_BOOL,
} value_t;

typedef struct {
  union {
    char *sval;
    double dval;
    int bool_val;
  };
  value_t type;
} value;

int number_cast(value *i);

void bool_cast(value *i);

void minus_number(value *i);

void bang_bool(value *i);

int equal_equal(value *left, value *right);

int plus(linmem *mem, value *dest, value *right);

int minus(value *dest, value *right);

int star(value *dest, value *right);

int slash(value *dest, value *right);

int less(value *left, value *right);

int greater(value *left, value *right);

void value_println(FILE *ofp, value i);
