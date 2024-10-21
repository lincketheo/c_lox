#include "value_hashtable.h"
#include "memory.h"
#include <string.h>

#define TABLE_SIZE 1000

unsigned int hash(char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash % TABLE_SIZE;
}

value_hashtable vhtbl_create() {
  value_hashtable vhtbl;
  vhtbl.mem = linmem_create();
  vhtbl.table = linmem_malloc(&vhtbl.mem, TABLE_SIZE * sizeof(entry *));
  memset(vhtbl.table, 0, TABLE_SIZE * sizeof(entry *));
  return vhtbl;
}

void vhtbl_free(value_hashtable *v) {
  free(v->table);
  v->table = NULL;
  linmem_free(&v->mem);
}

void vhtbl_insert(value_hashtable *vhtbl, char *key, value val) {
  unsigned int index = hash(key);
  entry *cur = vhtbl->table[index];

  while (cur != NULL) {
    if (strcmp(cur->ident, key) == 0) {
      cur->v = val;
    }
    cur = cur->next;
  }

  entry *next = linmem_malloc(&vhtbl->mem, sizeof *next);

  next->ident = key;
  next->v = val;
  next->next = vhtbl->table[index];

  vhtbl->table[index] = next;
}

value *vhtbl_get(value_hashtable *vhtbl, char *key) {
  unsigned int index = hash(key);
  entry *cur = vhtbl->table[index];

  while (cur != NULL) {
    if (strcmp(cur->ident, key) == 0) {
      return &cur->v;
    }
    cur = cur->next;
  }
  return NULL;
}
