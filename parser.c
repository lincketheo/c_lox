#include "parser.h"
#include "errors.h"
#include "expression.h"
#include "memory.h"
#include "statements.h"
#include "token.h"

#include <stdarg.h>
#include <string.h>

typedef struct {
  size_t cur;
  token_arr tokens;
  linmem mem;
} parser;

#define parser_ASSERT(p)                                                       \
  ASSERT(p);                                                                   \
  ASSERT((p)->cur <= (p)->tokens.len);

parser parser_create(token_arr arr) {
  return (parser){.tokens = arr, .cur = 0, .mem = linmem_create()};
}

static inline token_t parser_peek_tt(const parser *p) {
  parser_ASSERT(p);
  return p->tokens.tokens[p->cur].type;
}

static inline int parser_end(const parser *p) {
  parser_ASSERT(p);
  return parser_peek_tt(p) == TT_EOF;
}

static inline token parser_peek_t(const parser *p) {
  parser_ASSERT(p);
  return p->tokens.tokens[p->cur];
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
  if (parser_end(p))
    return TT_EOF;
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

/**
 * primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
 */
static expr *parse_primary(parser *p) {
  if (parser_match(p, 1, TRUE)) {
    expr *e = linmem_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_true());
    return e;
  }

  if (parser_match(p, 1, FALSE)) {
    expr *e = linmem_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_false());
    return e;
  }

  if (parser_match(p, 1, NIL)) {
    expr *e = linmem_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_NIL());
    return e;
  }

  if (parser_match(p, 1, STRING)) {
    token prev = parser_prev_t(p);

    ASSERT(prev.type == STRING);
    ASSERT(prev.str_val);

    expr *e = linmem_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_string(prev.str_val));
    return e;
  }

  if (parser_match(p, 1, NUMBER)) {
    token prev = parser_prev_t(p);

    ASSERT(prev.type == NUMBER);

    expr *e = linmem_malloc(&p->mem, sizeof *e);
    *e = expr_literal(lt_number(prev.n_val));
    return e;
  }

  if (parser_match(p, 1, IDENTIFIER)) {
    token prev = parser_prev_t(p);

    ASSERT(prev.type == IDENTIFIER);

    expr *e = linmem_malloc(&p->mem, sizeof *e);
    *e = expr_variable(prev.literal);
    return e;
  }

  if (parser_match(p, 1, LEFT_PAREN)) {
    expr *e = parse_expression(p);

    if (e == NULL)
      return NULL;

    if (parser_check(p, RIGHT_PAREN)) {
      parser_advance(p);
      expr *ret = linmem_malloc(&p->mem, sizeof *ret);
      *ret = expr_grouping(e);
      return ret;
    } else {
      token t = parser_peek_t(p);
      compile_error(t.line,
                    "Expected closing paren ')'. "
                    "Instead, got token of type: %s\n",
                    tttostr(t.type));
      return NULL;
    }
  }

  token t = parser_peek_t(p);
  compile_error(t.line, "Expected expression\n");
  parser_advance(p);
  return NULL;
}

/**
 * unary -> ( "!" | "-" ) unary | primary
 */
static expr *parse_unary(parser *p) {
  parser_ASSERT(p);

  if (parser_match(p, 2, BANG, MINUS)) {
    token_t prev = parser_prev_tt(p);
    expr *ret = linmem_malloc(&p->mem, sizeof *ret);

    expr *e = parse_unary(p);

    if (e == NULL)
      return NULL;

    *ret = expr_unary(unary_c(e, prev));
    return ret;
  } else {
    return parse_primary(p);
  }
}

/**
 * factor -> unary ( ( "/" | "*" ) unary )*
 */
static expr *parse_factor(parser *p) {
  parser_ASSERT(p);

  expr *e = parse_unary(p);

  if (e == NULL)
    return NULL;

  while (parser_match(p, 2, SLASH, STAR)) {
    token_t prev = parser_prev_tt(p);
    expr *right = parse_unary(p);

    if (right == NULL)
      return NULL;

    expr *ret = linmem_malloc(&p->mem, sizeof *ret);
    *ret = expr_binary(binary_c(e, prev, right));
    e = ret;
  }

  return e;
}

/**
 * term -> factor ( ( "+" | "-" ) factor )*
 */
static expr *parse_term(parser *p) {
  parser_ASSERT(p);

  expr *e = parse_factor(p);

  if (e == NULL)
    return NULL;

  while (parser_match(p, 2, PLUS, MINUS)) {
    token_t prev = parser_prev_tt(p);
    expr *right = parse_factor(p);

    if (right == NULL)
      return NULL;

    expr *ret = linmem_malloc(&p->mem, sizeof *ret);
    *ret = expr_binary(binary_c(e, prev, right));
    e = ret;
  }

  return e;
}

/**
 * comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
 */
static expr *parse_comparison(parser *p) {
  parser_ASSERT(p);

  expr *e = parse_term(p);

  if (e == NULL)
    return NULL;

  while (parser_match(p, 4, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL)) {
    token_t prev = parser_prev_tt(p);
    expr *right = parse_term(p);

    if (right == NULL)
      return NULL;

    expr *ret = linmem_malloc(&p->mem, sizeof *ret);
    *ret = expr_binary(binary_c(e, prev, right));
    e = ret;
  }

  return e;
}

/**
 * equality -> comparison ( ( "!=" | "==") comparison )*
 */
static expr *parse_equality(parser *p) {
  parser_ASSERT(p);

  expr *e = parse_comparison(p);

  if (e == NULL)
    return NULL;

  while (parser_match(p, 2, EQUAL_EQUAL, BANG_EQUAL)) {
    token_t prev = parser_prev_tt(p);
    expr *right = parse_comparison(p);

    if (right == NULL)
      return NULL;

    expr *ret = linmem_malloc(&p->mem, sizeof *ret);
    *ret = expr_binary(binary_c(e, prev, right));
    e = ret;
  }

  return e;
}

expr *parse_expression(parser *p) { return parse_equality(p); }

int parse_print_stmt(stmt *dest, parser *p) {
  parser_ASSERT(p);
  ASSERT(dest);

  expr *e = parse_expression(p);
  if (e == NULL) {
    return -1;
  }

  if (parser_match(p, 1, SEMICOLON)) {
    dest->type = ST_PRNT;
    dest->e = e;
    return 0;
  } else {
    token t = parser_peek_t(p);
    compile_error(t.line,
                  "Expected semicolon ';'. "
                  "Instead, got token of type: %s\n",
                  tttostr(t.type));
    return -1;
  }
}

int parse_expr_stmt(stmt *dest, parser *p) {
  parser_ASSERT(p);
  ASSERT(dest);

  expr *e = parse_expression(p);
  if (e == NULL) {
    return -1;
  }

  if (parser_match(p, 1, SEMICOLON)) {
    dest->type = ST_EXPR;
    dest->e = e;
    return 0;
  } else {
    token t = parser_peek_t(p);
    compile_error(t.line,
                  "Expected semicolon ';'. "
                  "Instead, got token of type: %s\n",
                  tttostr(t.type));
    return -1;
  }
}

int parse_stmt(stmt *dest, parser *p) {
  ASSERT(dest);
  parser_ASSERT(p);

  if (parser_match(p, 1, PRINT)) {
    return parse_print_stmt(dest, p);
  }
  return parse_expr_stmt(dest, p);
}

int parse_var_decl(stmt *dest, parser *p) {
  if (!parser_match(p, 1, IDENTIFIER)) {
    runtime_error("Expected variable name\n");
    return -1;
  }
  token t = parser_prev_t(p);

  expr *initializer = NULL;
  if (parser_match(p, 1, EQUAL)) {
    initializer = parse_expression(p);
  }

  if (!parser_match(p, 1, SEMICOLON)) {
    runtime_error("Expected ';' after variable declaration\n");
    return -1;
  }

  dest->type = ST_DECL;
  dest->e = initializer;
  dest->ident_name = t.literal;

  return 0;
}

int parse_decl(stmt *dest, parser *p) {
  ASSERT(dest);
  parser_ASSERT(p);

  if (parser_match(p, 1, VAR)) {
    if (parse_var_decl(dest, p)) {
      parser_synchronize(p);
      return -1;
    }
    return 0;
  }
  return parse_stmt(dest, p);
}

stmt_arr parse_tokens(token_arr arr) {
  parser p = parser_create(arr);
  stmt_arr ret = stmt_arr_create();

  while (!parser_end(&p)) {
    stmt s;
    if (parse_decl(&s, &p) == 0)
      stmt_arr_push(&ret, s);
  }

  return ret;
}
