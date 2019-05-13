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
  TK_EQ,                        ///< for == token
  TK_NE,                        ///< for != token
  TK_LE,                        ///< for <= token
  TK_GE,                        ///< for >= token
};

/** Token type */
typedef struct Token {
  int ty;                       ///< value of token type
  int val;                      ///< value of token (if ty == TK_NUM)
  char* input;                  ///< token string (for error message)
} Token;

/** values of Node types */
enum {
  ND_NUM = 256,                 ///< for a number node
  ND_EQ,                        ///< for == node
  ND_NE,                        ///< for != node
  ND_LE,                        ///< for <= node
};

/** Node type - Node of Abstract Syntax Tree (AST) */
typedef struct Node {
  int ty;                       ///< value of node type
  struct Node* lhs;             ///< left-hand node
  struct Node* rhs;             ///< right-hand node
  int val;                      ///< value of node (if ty == ND_NUM)
} Node;

#define MAX_TOKENS (100)
Token tokens[MAX_TOKENS];       ///< List of tokens
int pos = 0;                    ///< Index of the next token

/** Print error message then exit with error code. */
void error(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "error:");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

/** Construct list of tokens from a string. */
void tokenize(char* p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (strncmp(p, "==", 2) == 0) {
      tokens[i].ty = TK_EQ;
      tokens[i].input = p;
      i++;
      p+=2;
      continue;
    }
    if (strncmp(p, "!=", 2) == 0) {
      tokens[i].ty = TK_NE;
      tokens[i].input = p;
      i++;
      p+=2;
      continue;
    }
    if (strncmp(p, "<=", 2) == 0) {
      tokens[i].ty = TK_LE;
      tokens[i].input = p;
      i++;
      p+=2;
      continue;
    }
    if (strncmp(p, ">=", 2) == 0) {
      tokens[i].ty = TK_GE;
      tokens[i].input = p;
      i++;
      p+=2;
      continue;
    }
    if (*p == '<' || *p == '>' ||
        *p == '+' || *p == '-' ||
        *p == '*' || *p == '/' ||
        *p == '(' || *p == ')') {
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

/** Create a binary operator node. */
Node* new_node(int ty, Node* lhs, Node* rhs)
{
  Node* node = (Node*) malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

/** Create a number node. */
Node* new_node_num(int val)
{
  Node* node = (Node*) malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

/** Consume the next token if it was expected token type. */
int consume(int ty)
{
  if (tokens[pos].ty != ty) {
    return 0;
  }
  pos++;
  return 1;
}

// --- parsers

Node* expr(void);
Node* equality(void);
Node* relational(void);
Node* add(void);
Node* mul(void);
Node* unary(void);
Node* term(void);

Node* expr(void)
{
  Node* node = equality();
  return node;
}

Node* equality(void)
{
  Node* node = relational();
  for (;;) {
    if (consume(TK_EQ)) {
      node = new_node(ND_EQ, node, relational());
    }
    else if (consume(TK_NE)) {
      node = new_node(ND_NE, node, relational());
    }
    else {
      return node;
    }
  }
}

Node* relational(void)
{
  Node* node = add();
  for (;;) {
    if (consume('<')) {
      node = new_node('<', node, add());
    }
    else if (consume(TK_LE)) {
      node = new_node(ND_LE, node, add());
    }
    else if (consume('>')) {
      node = new_node('<', add(), node);
    }
    else if (consume(TK_GE)) {
      node = new_node(ND_LE, add(), node);
    }
    else {
      return node;
    }
  }
}

Node* add(void)
{
  Node* node = mul();
  for (;;) {
    if (consume('+')) {
      node = new_node('+', node, mul());
    }
    else if (consume('-')) {
      node = new_node('-', node, mul());
    }
    else {
      return node;
    }
  }
}

Node* mul(void)
{
  Node* node = unary();
  for (;;) {
    if (consume('*')) {
      node = new_node('*', node, unary());
    }
    else if (consume('/')) {
      node = new_node('/', node, unary());
    }
    else {
      return node;
    }
  }
}

Node* unary(void)
{
  if (consume('+')) {
    return term();
  }
  if (consume('-')) {
    return new_node('-', new_node_num(0), term());
  }
  return term();
}

Node* term(void)
{
  if (consume('(')) {
    Node* node = add();
    if (!consume(')')) {
      error("Paren not closed: %s", tokens[pos].input);
    }
    return node;
  }
  if (tokens[pos].ty == TK_NUM) {
    return new_node_num(tokens[pos++].val);
  }
  error("A token neither a number nor parens: %s", tokens[pos].input);
}

// ---- code generator

void gen(Node* node)
{
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->ty) {
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case '<':
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case '+':
    printf("  add rax, rdi\n");
    break;
  case '-':
    printf("  sub rax, rdi\n");
    break;
  case '*':
    printf("  mul rdi\n");
    break;
  case '/':
    printf("  mov rdx, 0\n");
    printf("  div rdi\n");
  }

  printf("  push rax\n");
}

// ----

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "Illegal number of arguments\n");
    return 1;
  }

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
