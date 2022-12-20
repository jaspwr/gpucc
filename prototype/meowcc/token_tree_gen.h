#pragma once
#include "inc.h"

//potentially change this to enum
#define EMPTY_TOKEN 0
#define ANYTHING_TOKEN 1
#define ANY_IDENTIFIER_TOKEN 2
#define ANY_LITERAL_TOKEN 3

struct token{
    int outp;
    std::string string_ = "";
    int parse_tree_extra_pointer;
};

struct tree_row_item{
    unsigned char x_jump = 0;   
    unsigned char exit_id = 0;
    unsigned char b = 0;
    unsigned char a = 0;
};

struct tree_row{
    int depth;
    char leading_char;
    tree_row_item items[256];
};

struct token_tree
{
    tree_row *data;
    int height;
    int token_count;
};


extern int parse_token_with_tree(token_tree* tt, char* str);
extern int add_token(const char* str, int outp, int parse_tree_extra_pointer);
extern void flush_tree(bool reset_len);
token_tree token_tree_gen();
const int len = 400;
extern token tokens[len];