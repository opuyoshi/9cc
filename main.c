#include "9cc.h"

int main(int argc, char **argv){
    if(argc != 2)
        error("%s: invalid number of arguments", argv[0]);

    user_input = argv[1];
    token = tokenize();
    program();  // parse
    codegen();

    return 0;
}
