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