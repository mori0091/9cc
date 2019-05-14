/* -*- mode:c++; coding:utf-8-unix -*- */

#include "9cc.h"

/** Print error message then exit with error code. */
noreturn void error(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "error:");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// ----

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "Illegal number of arguments\n");
    return 1;
  }

  // self test
  if (strcmp("--test", argv[1]) == 0) {
    runtest();
    return 0;
  }

  tokens = new_vector();
  tokenize(argv[1]);
  Node* node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
