#ifndef CC_H
#define CC_H

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Token type
typedef enum{
    TK_RESERVED, //symbol
    TK_NUM,  //integer
    TK_EOF,  // End Of File
}TokenKind;

typedef struct Token Token;
struct Token{
    TokenKind kind; //Token type
    Token *next;  //Next token
    int val;  //If token type is TK_NUM, input integer
    char *str;  //Token character
    int len;  //Token length
};


void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number(void);
bool at_eof(void);
Token *tokenize(void);

extern char *user_input;
extern Token *token;

typedef enum{
    ND_ADD,  // +
    ND_SUB,  // -
    ND_MUL,  // *
    ND_DIV,  // -
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_NUM,  // Integer
} NodeKind;

//Define Node
typedef struct Node Node;
struct Node{
    NodeKind kind;  // Node kind
    Node *lhs;  //left-hand side
    Node *rhs;  //right-hand side
    int val;  //used if kind == ND_NUM
};

Node *expr(void);

void codegen(Node *node);

#endif
