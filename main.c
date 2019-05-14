/* -*- coding:utf-8-unix -*- */

#include "9cc.h"

/** Print error message then exit with error code. */
noreturn void error(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "error:");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// ----

int main(int argc, char **argv) {
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

  // tokenize, then parse
  tokenize(argv[1]);
  program();

  // generate assembly code
  // - header
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // - prologue
  //   allocate 26 local variables 'a'-'z'
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n"); // 208 bytes = 8 bytes * 26

  // - generate code for each statement
  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    // pop result of the last expression of the statement.
    printf("  pop rax\n");
  }

  // - epilogue
  //   deallocate 26 local variables 'a'-'z'
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  //   note: 'rax' has already the result of the last expression,
  //   and the value of 'rax' shall be the return value.
  printf("  ret\n");
  return 0;
}
