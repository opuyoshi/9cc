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

//Token information
struct Token{
    TokenKind kind; //Token type
    Token *next;  //Next token
    int val;  //If token type is TK_NUM, input integer
    char *str;  //Token character
    int len;  //Token length
};

char *user_input; //input program
Token *token; //current token

void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

//Report error and error place
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

//If token is expected symbol, increment token pointer address
//and return true, otherwise return false.
bool consume(char *op){
    if(token->kind != TK_RESERVED || 
       strlen(op) != token->len ||
       memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

//If token is expected symbol, increment token pointer address,
//otherwise, report error place.
void expect(char *op){
    if(token->kind != TK_RESERVED || strlen(op) != token->len ||
       memcmp(token->str, op, token->len))
        error_at(token->str, "expected '%s'", op);
    token = token->next;
}

//If token is expected number, read integer and increment token pointer adress,
//otherwise report error place.
int expect_number(){
    if(token->kind != TK_NUM)
        error_at(token->str, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}


bool at_eof(){
    return token->kind == TK_EOF;
}

// Compare character to operator
bool startswith(char *p, char *q){
    return memcmp(p, q, strlen(q)) == 0;
}

//Make new token and link to current(cur) token
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

//Tokenize user_input
Token *tokenize(){
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        //Skip space character
        if(isspace(*p)){
            p++;
            continue;
        }

        // multi-letter operators
        if(startswith(p, "==") || startswith(p, "!=") ||
           startswith(p, "<=") || startswith(p, ">=")){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // single-latter operators
        if(strchr("+-*/()<>", *p)){
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        // integer
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "invalid token");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

//Define Node type
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

typedef struct Node Node;

//Define Node
struct Node{
    NodeKind kind;  // Node kind
    Node *lhs;  //left-hand side
    Node *rhs;  //right-hand side
    int val;  //used if kind == ND_NUM
};

//Allocate Node memory and input Node type
Node *new_node(NodeKind kind){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

//Make new symbol("+-*/()") node
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

//Make new integer node
Node *new_num(int val){
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// expr = equality
Node *expr(){
    return equality();
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(){
    Node *node = relational();

    for(;;){
        if(consume("=="))
            node = new_binary(ND_EQ, node, relational());
        else if(consume("!="))
            node = new_binary(ND_NE, node, relational());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(){
    Node *node = add();

    for(;;){
        if(consume("<"))
            node = new_binary(ND_LT, node, add());
        else if(consume("<="))
            node = new_binary(ND_LE, node, add());
        else if(consume(">"))
            node = new_binary(ND_LT, add(), node);
        else if(consume(">="))
            node = new_binary(ND_LE, add(), node);
        else
            return node;
    }

}

// add = mul ("+" mul | "-" mul)*
Node *add(){
    Node *node = mul();

    for(;;){
        if(consume("+"))
            node = new_binary(ND_ADD, node, mul());
        else if(consume("-"))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul(){
    Node *node = unary();

    for(;;){
        if(consume("*"))
            node = new_binary(ND_MUL, node, unary());
        else if(consume("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ("+" | "-")? primary
Node *unary(){
    if(consume("+"))  // skip "+"
        return unary(); //recursion for multiple unary operators
    if(consume("-"))
        return new_binary(ND_SUB, new_num(0), unary()); // recursion for multiple unary operators
    return primary();
}

// primary = "(" expr ")" | num
Node *primary(){
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }

    return new_num(expect_number());
}

//Generate nodes and calculate each node
void gen(Node *node){
    if(node->kind == ND_NUM){
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind){
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
        case ND_EQ:
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_NE:
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LT:
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break; 
        case ND_LE:
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
    }

    printf("    push rax\n");
}

int main(int argc, char **argv){
    if (argc != 2)
        error("%s: invalid number of arguments", argv[0]);

    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    //output first half of assembly
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}
