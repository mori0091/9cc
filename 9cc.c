/* mode:c++; coding:utf-8-unix */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** values of token types */
enum {
  TK_NUM = 256,                 ///< for a number token
  TK_EOF,                       ///< for end-of-file token
};

/** Token type */
typedef struct {
  int ty;                       ///< value of token type
  int val;                      ///< value of token (if ty == TK_NUM)
  char* input;                  ///< token string (for error message)
} Token;

#define MAX_TOKENS (100)
Token tokens[MAX_TOKENS];

void error(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void tokenize(char* p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }
    error("Cannot tokenize: %s", p);
    exit(1);
  }
  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "Illegal number of arguments\n");
    return 1;
  }

  tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  if (tokens[0].ty != TK_NUM) {
    error("The first term is not a number");
  }
  printf("  mov rax, %d\n", tokens[0].val);

  int i = 1;
  while (tokens[i].ty != TK_EOF) {
    if (tokens[i].ty == '+') {
      i++;
      if (tokens[i].ty != TK_NUM) {
        error("Unexpected token: %s", tokens[i].input);
      }
      printf("  add rax, %d\n", tokens[i].val);
      i++;
      continue;
    }
    if (tokens[i].ty == '-') {
      i++;
      if (tokens[i].ty != TK_NUM) {
        error("Unexpected token: %s", tokens[i].input);
      }
      printf("  sub rax, %d\n", tokens[i].val);
      i++;
      continue;
    }
    error("Unexpected token: %s", tokens[i].input);
  }
  printf("  ret\n");
  return 0;
}
