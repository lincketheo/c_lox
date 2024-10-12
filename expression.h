#pragma once

#include <stdio.h>
typedef struct expr_s expr;

///////////////////////////////////////
////////////// Section Literal
typedef enum { LT_NUMBER, LT_STRING, LT_TRUE, LT_FALSE, LT_NIL } literal_t;

typedef struct {
  union {
    double dval;
    char *sval;
  };
  literal_t type;
} literal;

static inline literal lt_number(double v) {
  return (literal){.type = LT_NUMBER, .dval = v};
}

static inline literal lt_string(char *v) {
  return (literal){.type = LT_STRING, .sval = v};
}

static inline literal lt_true() {
  return (literal){
      .type = LT_TRUE,
  };
}

static inline literal lt_false() {
  return (literal){
      .type = LT_FALSE,
  };
}

static inline literal lt_NIL() {
  return (literal){
      .type = LT_NIL,
  };
}

///////////////////////////////////////
////////////// Section Unary
typedef enum { UT_MINUS, UT_BANG } unary_t;

typedef struct {
  unary_t type;
  expr *e;
} unary;

static inline unary ur_minus(expr *e) {
  return (unary){
      .type = UT_MINUS,
      .e = e,
  };
}

static inline unary ur_bang(expr *e) {
  return (unary){
      .type = UT_BANG,
      .e = e,
  };
}

///////////////////////////////////////
////////////// Section Operator
typedef enum {
  OT_EQUAL_EQUAL,
  OT_BANG_EQUAL,
  OT_LESS,
  OT_LESS_EQUAL,
  OT_GREATER,
  OT_GREATER_EQUAL,
  OT_PLUS,
  OT_MINUS,
  OT_STAR,
  OT_SLASH
} operator_t;

///////////////////////////////////////
////////////// Section Binary
typedef struct {
  expr *left;
  operator_t op;
  expr *right;
} binary;

static inline binary binary_c(expr *left, operator_t op, expr *right) {
  return (binary){
      .left = left,
      .op = op,
      .right = right,
  };
}

///////////////////////////////////////
////////////// Section Expression
typedef enum { ET_LITERAL, ET_UNARY, ET_BINARY, ET_GROUPING } expr_t;

struct expr_s {
  union {
    literal l;
    unary u;
    binary b;
    expr *g;
  };
  expr_t type;
};

static inline expr expr_literal(literal l) {
  return (expr){.type = ET_LITERAL, .l = l};
}

static inline expr expr_unary(unary u) {
  return (expr){.type = ET_UNARY, .u = u};
}

static inline expr expr_binary(binary b) {
  return (expr){.type = ET_BINARY, .b = b};
}

static inline expr expr_grouping(expr *e) {
  return (expr){.type = ET_GROUPING, .g = e};
}

int fprintln_expr(FILE *ofp, expr *e);
