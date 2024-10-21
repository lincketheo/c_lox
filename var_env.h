#pragma once

#include "errors.h"
#include "value_hashtable.h"

typedef struct {
  value_hashtable values;
} var_env;

static inline var_env var_env_create() {
  var_env ret;
  ret.values = vhtbl_create();
  return ret;
}

static inline void var_env_define(var_env *env, char *ident, value v) {
  vhtbl_insert(&env->values, ident, v);
}

static inline value *var_env_get(var_env *env, char *ident) {
  value *ret = vhtbl_get(&env->values, ident);
  if (ret == NULL) {
    runtime_error("Undefined variable: %s\n", ident);
    return NULL;
  }
  return ret;
}
