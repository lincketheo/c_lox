#pragma once

#include "memory.h"
#include "value.h"

typedef struct entry {
  char *ident;
  value v;
  struct entry *next;
} entry;

typedef struct {
  entry **table;
  linmem mem;
} value_hashtable;

value_hashtable vhtbl_create();

void vhtbl_free(value_hashtable *v);

void vhtbl_insert(value_hashtable *vhtbl, char *key, value val);

value *vhtbl_get(value_hashtable *vhtbl, char *key);
