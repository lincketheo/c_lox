#pragma once

#include "expression.h"
#include "statements.h"
#include "var_env.h"

int interpret_stmts(linmem *mem, stmt_arr *s, var_env *env);
