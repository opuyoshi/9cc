#include "9cc.h"

//Allocate Node memory and input Node type
static Node *new_node(NodeKind kind){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

//Make new symbol("+-*/()") node
static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

//Make new integer node
static Node *new_num(int val){
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

static Token *consume_ident(){
    if(token->kind != TK_IDENT)
        return NULL;
    Token *t = token;
    token = token->next;
    return t;
}

Node *code[100];  //store lines

static Node *stmt(void);
static Node *expr(void);
static Node *assign(void);
static Node *equality(void);
static Node *relational(void);
static Node *add(void);
static Node *mul(void);
static Node *unary(void);
static Node *primary(void);

void program(){
    int i = 0;
    while(!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

// stmt = expr ";"
static Node *stmt(){
    Node *node = expr();
    expect(";");
    return node;
}

// expr = assign
static Node *expr(){
    return assign();
}

// assign = equality ("=" assign)?
Node *assign(){
    Node *node = equality();
    if(consume("="))
        node = new_binary(ND_ASSIGN, node, assign());
    return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality(void){
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
static Node *relational(void){
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
static Node *add(void){
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
static Node *mul(void){
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
static Node *unary(void){
    if(consume("+"))  // skip "+"
        return unary(); //recursion for multiple unary operators
    if(consume("-"))
        return new_binary(ND_SUB, new_num(0), unary()); // recursion for multiple unary operators
    return primary();
}

// primary = num | ident | "(" expr ")"
static Node *primary(void){
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if(tok){
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;
        node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }

    return new_num(expect_number());
}

