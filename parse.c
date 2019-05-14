/* -*- mode:c++; coding:utf-8-unix -*- */

#include "9cc.h"

// ---- tokenizer

Vector* tokens = NULL;          ///< List of tokens
int pos = 0;                    ///< Index of the next token

void tokenize(char* p)
{
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (islower(*p)) {
      Token* token = malloc(sizeof(Token));
      token->ty = TK_IDENT;
      token->input = p;
      vec_push(tokens, token);
      p++;
      continue;
    }
    if (strncmp(p, "==", 2) == 0) {
      Token* token = malloc(sizeof(Token));
      token->ty = TK_EQ;
      token->input = p;
      vec_push(tokens, token);
      p+=2;
      continue;
    }
    if (strncmp(p, "!=", 2) == 0) {
      Token* token = malloc(sizeof(Token));
      token->ty = TK_NE;
      token->input = p;
      vec_push(tokens, token);
      p+=2;
      continue;
    }
    if (strncmp(p, "<=", 2) == 0) {
      Token* token = malloc(sizeof(Token));
      token->ty = TK_LE;
      token->input = p;
      vec_push(tokens, token);
      p+=2;
      continue;
    }
    if (strncmp(p, ">=", 2) == 0) {
      Token* token = malloc(sizeof(Token));
      token->ty = TK_GE;
      token->input = p;
      vec_push(tokens, token);
      p+=2;
      continue;
    }
    if (*p == '<' || *p == '>' ||
        *p == '+' || *p == '-' ||
        *p == '*' || *p == '/' ||
        *p == '(' || *p == ')') {
      Token* token = malloc(sizeof(Token));
      token->ty = *p;
      token->input = p;
      vec_push(tokens, token);
      p++;
      continue;
    }
    if (isdigit(*p)) {
      Token* token = malloc(sizeof(Token));
      token->ty = TK_NUM;
      token->input = p;
      token->val = strtol(p, &p, 10);
      vec_push(tokens, token);
      continue;
    }
    error("Cannot tokenize: %s", p);
    exit(1);
  }
  Token* token = malloc(sizeof(Token));
  token->ty = TK_EOF;
  token->input = p;
  vec_push(tokens, token);
}

// --- Abstract Syntax Tree (AST)

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
  Token* token = (Token*)tokens->data[pos];
  if (token->ty != ty) {
    return 0;
  }
  pos++;
  return 1;
}

// --- parsers

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
    Node* node = expr();
    if (!consume(')')) {
      error("Paren not closed: %s",
            ((Token*)tokens->data[pos])->input);
    }
    return node;
  }
  if (((Token*)tokens->data[pos])->ty == TK_NUM) {
    return new_node_num(((Token*)tokens->data[pos++])->val);
  }
  error("A token neither a number nor parens: %s",
        ((Token*)tokens->data[pos])->input);
}
