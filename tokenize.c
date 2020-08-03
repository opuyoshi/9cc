#include "9cc.h"

char *user_input;
Token *token;

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
static bool startswith(char *p, char *q){
    return memcmp(p, q, strlen(q)) == 0;
}

//Make new token and link to current(cur) token
static Token *new_token(TokenKind kind, Token *cur, char *str, int len){
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
        if(strchr("+-*/()<>;=", *p)){
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

        //identifier
        if(isalpha(*p)){
            char *q = p++;
            while(isalnum(*p) || strchr("_", *p))
                p++;
            cur = new_token(TK_IDENT, cur, q, p-q);
            continue;
        }

        error_at(p, "invalid token");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
