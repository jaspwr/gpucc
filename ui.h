#pragma once
#include "inc.h"
#include "compiler.h"

#define PRINT_SUCCESS   0
#define PRINT_INFO      1
#define PRINT_WARNING   2
#define PRINT_ERROR     3


void print(int type, const char* message);
void print(int type, int message);
char* parse_args(int argc, char** argv);

int parse_dbg_mode(char* str);