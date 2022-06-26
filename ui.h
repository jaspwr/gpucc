#pragma once
#include "inc.h"
#include "compiler.h"

#define PRINT_SUCCESS   0
#define PRINT_INFO      1
#define PRINT_WARNING   2
#define PRINT_ERROR     3


void print(int type, const char* message);
void print(int type, int message);
void exec_with_args(int argc, char** argv);