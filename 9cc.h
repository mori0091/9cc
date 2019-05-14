/* -*- mode:c++; coding:utf-8-unix -*- */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdnoreturn.h>

// ---- utility

noreturn void error(const char *fmt, ...);


// ---- container

typedef struct Vector {
  void** data;
  int capacity;
  int len;
} Vector;

/** Create new vector */
Vector* new_vector(void);
/** Push back an element to the vector */
void vec_push(Vector* vec, void* elem);

/** Assertion (unit-test) */
void expect(int line, int expected, int actual);
/** Run self-test */
void runtest(void);


// ---- tokenizer

/** values of token types */
enum {
      TK_NUM = 256,                 ///< for a number token
      TK_EOF,                       ///< for end-of-file token
      TK_EQ,                        ///< for == token
      TK_NE,                        ///< for != token
      TK_LE,                        ///< for <= token
      TK_GE,                        ///< for >= token
      TK_IDENT,                     ///< for identifier token
};

/** Token type */
typedef struct Token {
  int ty;                       ///< value of token type
  int val;                      ///< value of token (if ty == TK_NUM)
  char* input;                  ///< token string (for error message)
} Token;

extern Vector* tokens;          ///< List of tokens
extern int pos;                 ///< Index of the next token

/** Construct list of tokens from a string. */
void tokenize(char* p);


// --- Abstract Syntax Tree (AST)

/** values of Node types */
enum {
      ND_NUM = 256,                 ///< for a number node
      ND_EQ,                        ///< for == node
      ND_NE,                        ///< for != node
      ND_LE,                        ///< for <= node
      ND_IDENT,                     ///< for identifier ndoe
};

/** Node type - Node of Abstract Syntax Tree (AST) */
typedef struct Node {
  int ty;                       ///< value of node type
  struct Node* lhs;             ///< left-hand node
  struct Node* rhs;             ///< right-hand node
  int val;                      ///< value of node (if ty == ND_NUM)
  char name;                    ///< name of identifier (if ty == ND_INDENT)
} Node;

/** Create a binary operator node. */
Node* new_node(int ty, Node* lhs, Node* rhs);

/** Create a number node. */
Node* new_node_num(int val);

/** Create a identifier node. */
Node* new_node_ident(char name);

/** Consume the next token if it was expected token type. */
int consume(int ty);


// ---- parsers

extern Node* code[100];         ///< List of statement nodes

// ~~~BNF
//    program = { stmt }
//       stmt = expr ";"
//       expr = assign
//     assign = equality { "=" assign }
//   equality = relational { ( "==" | "!=" ) relational }
// relational = add { ( "<" | "<=" | ">" | ">=" ) add }
//        add = mul { ( "+" | "-" ) mul }
//        mul = unary { ( "*" | "/" ) unary }
//      unary = [ "+" | "-" ] term
//       term = num | ident | "(" expr ")"
// ~~~
void program(void);
Node* stmt(void);
Node* expr(void);
Node* assign(void);
Node* equality(void);
Node* relational(void);
Node* add(void);
Node* mul(void);
Node* unary(void);
Node* term(void);


// ---- code generator

void gen_lval(Node* node);
void gen(Node* node);
