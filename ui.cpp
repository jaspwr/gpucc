#include "ui.h"

const char* indicators[]{
    "S",
    "i",
    "w",
    "E"
};

void print(int type, const char* message){
    printf("[%s] ",indicators[type]);
    printf("%s\n",message);
}

void print(int type, int message){
    printf("[%s] ",indicators[type]);
    printf("%d\n",message);
}

void exec_with_args(int argc, char** argv){
    for(int i = 1; i < argc; i++){
        if(argv[i][0] = '-'){
            //is flag
            compiler::regens_lang = true;
        }
    }
}