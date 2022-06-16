#pragma once
#include "inc.h"

struct token{
    int outp;
    std::string string_ = "";
};

struct tree_row_item{
    unsigned char x_jump = 0;   
    unsigned char exit_id = 0;
    unsigned char b = 255;
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

extern int add_token(const char* str);
extern void flush_tree(bool reset_len);
token_tree token_tree_gen();
const int len = 400;
extern token tokens[len];