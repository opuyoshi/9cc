#include "9cc.h"

static int counter = 0;  // make unique label

static void gen_lval(Node *node){
    if(node->kind != ND_LVAR)
        error("Assignment on leftvalue is not value.");
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

static void gen(Node *node){

    switch(node->kind){
        case ND_RETURN:  // return
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            return;
        case ND_NUM:  // integer
            printf("    push %d\n", node->val);
            return;
        case ND_LVAR:  // local value
            gen_lval(node);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_ASSIGN:  // =
            gen_lval(node->lhs);
            gen(node->rhs);
            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;
        case ND_IF:  // if
            counter += 1;
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            if(node->els){  // else
                printf("    je .Lelse%d\n", counter);
                gen(node->then);
                printf("    jmp .Lend%d\n", counter);
                printf("    .Lelse%d:\n", counter);
                gen(node->els);
                printf("    .Lend%d:\n", counter);
            }
            else{  // only if statement
                printf("    je .Lend%d\n", counter);
                gen(node->then);
                printf("    .Lend%d:\n", counter);
            }
            return;
        case ND_WHILE:  // while
            counter += 1;
            printf("    .Lbegin%d:\n", counter);
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n", counter);
            gen(node->then);
            printf("    jmp .Lbegin%d\n", counter);
            printf("    .Lend%d:", counter);
            return;
        case ND_FOR:  // for
            counter += 1;
            if(node->init)
                gen(node->init);
            printf("    .Lbegin%d:\n", counter);
            if(node->cond){
                gen(node->cond);
                printf("    pop rax\n");
                printf("    cmp rax, 0\n");
                printf("    je .Lend%d\n", counter);
            }
            gen(node->then);
            if(node->inc)
                gen(node->inc);
            printf("    jmp .Lbegin%d\n", counter);
            printf("    .Lend%d:\n", counter);
            return;
        case ND_BLOCK:  // {...}
            for(Node *n = node->block; n; n = n->next)
                gen(n);
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind){
        case ND_ADD:  // +
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:  // -
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:  // *
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:  // /
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
        case ND_EQ:  // =
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_NE:  // !=
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LT:  // <
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LE:  // <=
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
    }

    printf("    push rax\n");
}

void codegen(){
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    //prologue
    //secure memory area for 26 values
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");  // 26 *8 = 208

    //generate code from the first line
    for(int i = 0; code[i]; i++){
        gen(code[i]);

        //pop a generated value from a equation in order to avoid stack overflow
        printf("    pop rax\n");
    }

    //epilogue
    //a result from last equation is a return value
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
}
