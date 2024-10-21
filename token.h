#pragma once

#include "errors.h"
#include "memory.h"
#include <stdlib.h>

#define WHITESPACE_C                                                           \
  ' ' : case '\r':                                                             \
  case '\t'

typedef enum {
  // Single-character tokens.
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or two character tokens.
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals.
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  TT_EOF
} token_t;

const char *tttostr(token_t);

#define ttforeach(func)                                                        \
  func(LEFT_PAREN);                                                            \
  func(RIGHT_PAREN);                                                           \
  func(LEFT_BRACE);                                                            \
  func(RIGHT_BRACE);                                                           \
  func(COMMA);                                                                 \
  func(DOT);                                                                   \
  func(MINUS);                                                                 \
  func(PLUS);                                                                  \
  func(SEMICOLON);                                                             \
  func(SLASH);                                                                 \
  func(STAR);                                                                  \
  func(BANG);                                                                  \
  func(BANG_EQUAL);                                                            \
  func(EQUAL);                                                                 \
  func(EQUAL_EQUAL);                                                           \
  func(GREATER);                                                               \
  func(GREATER_EQUAL);                                                         \
  func(LESS);                                                                  \
  func(LESS_EQUAL);                                                            \
  func(IDENTIFIER);                                                            \
  func(STRING);                                                                \
  func(NUMBER);                                                                \
  func(AND);                                                                   \
  func(CLASS);                                                                 \
  func(ELSE);                                                                  \
  func(FALSE);                                                                 \
  func(FUN);                                                                   \
  func(FOR);                                                                   \
  func(IF);                                                                    \
  func(NIL);                                                                   \
  func(OR);                                                                    \
  func(PRINT);                                                                 \
  func(RETURN);                                                                \
  func(SUPER);                                                                 \
  func(THIS);                                                                  \
  func(TRUE);                                                                  \
  func(VAR);                                                                   \
  func(WHILE);                                                                 \
  func(TT_EOF)

typedef struct {
  union {
    // TODO
    char *str_val;
    double n_val;
  };
  char *literal;
  int line;
  token_t type;
} token;

typedef struct {
  token *tokens;
  size_t len;
  size_t cap;
  linmem mem; // Used to store c strings and literals
} token_arr;

#define token_arr_ASSERT(t)                                                    \
  ASSERT(t);                                                                   \
  ASSERT((t)->len <= (t)->cap);                                                \
  ASSERT((t)->tokens);                                                         \
  ASSERT((t)->cap > 0)

void token_arr_print(token_arr *t);

token_arr token_arr_create();

void token_arr_free(token_arr *arr);

void token_arr_push(token_arr *t, const char *loc, size_t len, token_t type,
                    int line);
