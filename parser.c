#include "errors.h"
#include "expression.h"
#include "memory.h"
#include "token.h"
#include <string.h>

typedef struct {
  size_t cur;
  token_arr tokens;
  memstack mem;
} parser;

#define parser_ASSERT(p)                                                       \
  ASSERT(p);                                                                   \
  ASSERT((p)->cur <= (p)->tokens.len);

parser parser_create(token_arr arr) {
  return (parser){.tokens = arr, .cur = 0, .mem = memstack_create()};
}

static inline token_t parser_peek_tt(const parser *p) {
  parser_ASSERT(p);
  return p->tokens.tokens[p->cur].type;
}

static inline token parser_peek_t(const parser *p) {
  parser_ASSERT(p);
  return p->tokens.tokens[p->cur];
}

static inline int parser_end(const parser *p) {
  parser_ASSERT(p);
  return parser_peek_tt(p) == TT_EOF;
}

static inline token_t parser_prev_tt(const parser *p) {
  parser_ASSERT(p);
  ASSERT(p->cur > 0);
  return p->tokens.tokens[p->cur - 1].type;
}

static inline token parser_prev_t(const parser *p) {
  parser_ASSERT(p);
  ASSERT(p->cur > 0);
  return p->tokens.tokens[p->cur - 1];
}

static inline int parser_check(parser *p, token_t t) {
  if (parser_end(p))
    return 0;
  return parser_peek_tt(p) == t;
}

static inline token_t parser_advance(parser *p) {
  parser_ASSERT(p);
  ASSERT(!parser_end(p));
  return p->tokens.tokens[p->cur++].type;
}

static int parser_match(parser *p, int count, ...) {
  parser_ASSERT(p);

  va_list args;
  va_start(args, count);

  for (int i = 0; i < count; ++i) {
    if (parser_check(p, va_arg(args, token_t))) {
      parser_advance(p);
      return 1;
    }
  }
  return 0;
}

static void parser_synchronize(parser *p) {
  parser_advance(p);

  while (!parser_end(p)) {
    if (parser_prev_tt(p) == SEMICOLON)
      return;
    switch (parser_peek_tt(p)) {
    case CLASS:
    case FUN:
    case VAR:
    case FOR:
    case IF:
    case WHILE:
    case PRINT:
    case RETURN:
      return;
    default:
      parser_advance(p);
    }
  }
}

expr *parse_expression(parser *p);

static expr *parse_primary(parser *p) {
  if (parser_match(p, TRUE)) {
    expr *e = memstack_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_true());
    return e;
  }

  if (parser_match(p, FALSE)) {
    expr *e = memstack_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_false());
    return e;
  }

  if (parser_match(p, NIL)) {
    expr *e = memstack_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_NIL());
    return e;
  }

  if (parser_match(p, STRING)) {
    token prev = parser_prev_t(p);
    ASSERT(prev.type == STRING);
    ASSERT(prev.str_val);
    expr *e = memstack_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_string(prev.str_val));
    return e;
  }

  if (parser_match(p, NUMBER)) {
    token prev = parser_prev_t(p);
    ASSERT(prev.type == NUMBER);
    expr *e = memstack_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_number(prev.n_val));
    return e;
  }

  if (parser_match(p, LEFT_PAREN)) {
    expr *e = parse_expression(p);
    if (parser_check(p, RIGHT_PAREN)) {
      parser_advance(p);
      expr *ret = memstack_malloc(&p->mem, sizeof *ret);
      *ret = expr_grouping(e);
      return ret;
    } else {
      token t = parser_peek_t(p);
      compile_error(t.line, "Expected closing paren ')'. Instead, got: %s",
                    t.literal);
      return NULL;
    }
  }
}

static expr *parse_unary(parser *p) {
  parser_ASSERT(p);

  if (parser_match(p, 2, BANG, MINUS)) {
    token_t prev = parser_prev_tt(p);
    expr *ret = memstack_malloc(&p->mem, sizeof *ret);
    *ret = expr_unary(unary_c(parse_unary(p), prev));
    return ret;
  } else {
    return parse_primary(p);
  }
}

static expr *parse_factor(parser *p) {
  parser_ASSERT(p);

  expr *e = parse_unary(p);

  while (parser_match(p, 2, SLASH, STAR)) {
    token_t prev = parser_prev_tt(p);
    expr *right = parse_unary(p);
    expr *ret = memstack_malloc(&p->mem, sizeof *ret);
    *ret = expr_binary(binary_c(e, prev, right));
    e = ret;
  }

  return e;
}

static expr *parse_term(parser *p) {
  parser_ASSERT(p);

  expr *e = parse_factor(p);

  while (parser_match(p, 2, PLUS, MINUS)) {
    token_t prev = parser_prev_tt(p);
    expr *right = parse_factor(p);
    expr *ret = memstack_malloc(&p->mem, sizeof *ret);
    *ret = expr_binary(binary_c(e, prev, right));
    e = ret;
  }

  return e;
}

static expr *parse_comparison(parser *p) {
  parser_ASSERT(p);

  expr *e = parse_term(p);

  while (parser_match(p, 4, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL)) {
    token_t prev = parser_prev_tt(p);
    expr *right = parse_term(p);
    expr *ret = memstack_malloc(&p->mem, sizeof *ret);
    *ret = expr_binary(binary_c(e, prev, right));
    e = ret;
  }

  return e;
}

static expr *parse_equality(parser *p) {
  parser_ASSERT(p);

  expr *e = parse_comparison(p);

  while (parser_match(p, 2, BANG, BANG_EQUAL)) {
    token_t prev = parser_prev_tt(p);
    expr *right = parse_comparison(p);
    expr *ret = memstack_malloc(&p->mem, sizeof *ret);
    *ret = expr_binary(binary_c(e, prev, right));
    e = ret;
  }

  return e;
}

expr *parse_expression(parser *p) { return parse_equality(p); }
