#include "scanner.h"
#include "errors.h"
#include "token.h"
#include <string.h>

typedef struct {
  const char *data;
  token_arr tokens;
  size_t start;
  size_t current;
  size_t line;
  int error;
} scanner_state;

#define scanner_state_ASSERT(s)                                                \
  ASSERT(s);                                                                   \
  ASSERT((s)->data);                                                           \
  ASSERT((s)->start <= (s)->current)

static scanner_state scanner_state_create(const char *data) {
  scanner_state ret;
  ret.data = data;
  ret.tokens = token_arr_create();
  ret.start = 0;
  ret.current = 0;
  ret.line = 1;
  ret.error = 0;
  return ret;
}

// Check if at the end
static inline int ss_end(const scanner_state *s) {
  scanner_state_ASSERT(s);
  return s->data[s->current] == '\0';
}

// Next char and advance
static char ss_next_ch(scanner_state *s) {
  scanner_state_ASSERT(s);
  ASSERT(!ss_end(s));
  return s->data[s->current++];
}

// Peek but don't advance
static char ss_peek_ch(const scanner_state *s) {
  scanner_state_ASSERT(s);
  return s->data[s->current];
}

// Peek twice but don't advance
static char ss_peek2_ch(const scanner_state *s) {
  scanner_state_ASSERT(s);
  if (ss_end(s) || s->data[s->current + 1] == '\0')
    return '\0';
  return s->data[s->current + 1];
}

// Advance if matches c, otherwise don't advance
static int ss_match_ch(scanner_state *s, char c) {
  scanner_state_ASSERT(s);
  if (ss_end(s))
    return 0;
  if (s->data[s->current] == c) {
    s->current++;
    return 1;
  } else {
    return 0;
  }
}

static void ss_parse_string(scanner_state *s) {
  char ch;
  while (!ss_end(s) && ss_peek_ch(s) != '\"') {
    if (ss_peek_ch(s) == '\n')
      s->line++;
    ss_next_ch(s);
  }

  if (ss_end(s)) {
    compile_error(s->line, "Unterminated string");
    s->error = 1;
    return;
  }

  ss_next_ch(s);
}

static inline int is_digit(char c) { return c <= '9' && c >= '0'; }

static void ss_parse_number(scanner_state *s) {
  while (is_digit(ss_peek_ch(s)))
    ss_next_ch(s);
  if (ss_peek_ch(s) == '.' && is_digit(ss_peek2_ch(s))) {
    ss_next_ch(s);
    while (is_digit(ss_peek_ch(s)))
      ss_next_ch(s);
  }
}

static inline int is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static inline int is_alpha_num(char c) { return is_alpha(c) || is_digit(c); }

static token_t ss_check_keyword(scanner_state *s) {
  size_t slen = s->current - s->start;

  // TODO - Hash it out
  if (slen == 3 && (strncmp(&s->data[s->start], "and", 3) == 0))
    return AND;
  if (slen == 5 && (strncmp(&s->data[s->start], "class", 5) == 0))
    return CLASS;
  if (slen == 4 && (strncmp(&s->data[s->start], "else", 4) == 0))
    return ELSE;
  if (slen == 5 && (strncmp(&s->data[s->start], "false", 5) == 0))
    return FALSE;
  if (slen == 3 && (strncmp(&s->data[s->start], "for", 3) == 0))
    return FOR;
  if (slen == 3 && (strncmp(&s->data[s->start], "fun", 3) == 0))
    return FUN;
  if (slen == 2 && (strncmp(&s->data[s->start], "if", 2) == 0))
    return IF;
  if (slen == 3 && (strncmp(&s->data[s->start], "nil", 3) == 0))
    return NIL;
  if (slen == 2 && (strncmp(&s->data[s->start], "or", 2) == 0))
    return OR;
  if (slen == 5 && (strncmp(&s->data[s->start], "print", 5) == 0))
    return PRINT;
  if (slen == 6 && (strncmp(&s->data[s->start], "return", 6) == 0))
    return RETURN;
  if (slen == 5 && (strncmp(&s->data[s->start], "super", 5) == 0))
    return SUPER;
  if (slen == 4 && (strncmp(&s->data[s->start], "this", 4) == 0))
    return THIS;
  if (slen == 4 && (strncmp(&s->data[s->start], "true", 4) == 0))
    return TRUE;
  if (slen == 3 && (strncmp(&s->data[s->start], "var", 3) == 0))
    return VAR;
  if (slen == 5 && (strncmp(&s->data[s->start], "while", 5) == 0))
    return WHILE;
  return -1;
}

static token_t ss_parse_ident(scanner_state *s) {
  while (is_alpha_num(ss_peek_ch(s)))
    ss_next_ch(s);

  token_t ret = ss_check_keyword(s);
  if (ret == -1) {
    return IDENTIFIER;
  } else {
    return ret;
  }
}

// Return -1 on no token parsed (might be error, might not)
static ssize_t ss_next_tt(scanner_state *s) {
  ASSERT(!ss_end(s));
  ASSERT(s->start == s->current);

  char c = ss_next_ch(s);

  switch (c) {
  case '(':
    return LEFT_PAREN;
  case ')':
    return RIGHT_PAREN;
  case '{':
    return LEFT_BRACE;
  case '}':
    return RIGHT_BRACE;
  case ',':
    return COMMA;
  case '.':
    return DOT;
  case '-':
    return MINUS;
  case '+':
    return PLUS;
  case ';':
    return SEMICOLON;
  case '*':
    return STAR;
  case '!': {
    if (ss_match_ch(s, '='))
      return BANG_EQUAL;
    return BANG;
  }
  case '=': {
    if (ss_match_ch(s, '='))
      return EQUAL_EQUAL;
    return EQUAL;
  }
  case '<': {
    if (ss_match_ch(s, '='))
      return LESS_EQUAL;
    return LESS;
  }
  case '>': {
    if (ss_match_ch(s, '='))
      return GREATER_EQUAL;
    return GREATER;
  }
  case '/': {
    if (ss_match_ch(s, '/')) {
      while (!ss_end(s) && ss_peek_ch(s) != '\n')
        ss_next_ch(s);
      return -1;
    } else {
      return SLASH;
    }
  }
  case WHITESPACE_C:
    return -1;

  case '\n':
    s->line++;
    return -1;

  case '\"':
    ss_parse_string(s);
    return STRING;

  default:
    if (is_digit(c)) {
      ss_parse_number(s);
      return NUMBER;
    } else if (is_alpha(c)) {
      return ss_parse_ident(s);
    }
    compile_error(s->line, "Unexpected char: %c\n", c);
    s->error = 1;
    return -1;
  }
}

static void ss_parse(token_arr *t, scanner_state *s) {
  scanner_state_ASSERT(s);
  ssize_t next;

  while (!ss_end(s)) {
    s->start = s->current;
    next = ss_next_tt(s);

    if (next != -1) {
      token_arr_push(t, &s->data[s->start], s->current - s->start, next,
                     s->line);
    }
  }

  token_arr_push(t, &s->data[s->start], s->current - s->start, TT_EOF, s->line);
}

token_arr scanner_parse_tokens(const char *data) {
  ASSERT(data);

  token_arr ret = token_arr_create();
  scanner_state s = scanner_state_create(data);
  ss_parse(&ret, &s);

  return ret;
}
