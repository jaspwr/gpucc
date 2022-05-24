#include<stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT_SUCCESS   0
#define PRINT_INFO      1
#define PRINT_WARNING   2
#define PRINT_ERROR     3

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