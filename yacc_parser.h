#pragma once
#include "token_tree_gen.h"
struct sentance{
    unsigned char* tokens;
};

struct abstract_token{
    unsigned char identifier;
    sentance* handles;
};

struct cst_rgba_pixel
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 255;
};

#define CST_ROW_WIDTH 256
struct cst_row
{
    cst_rgba_pixel vals[CST_ROW_WIDTH];
};


struct cst{
    //abstract_token* abstract_tokens;
    cst_row rows[512];
    int height;
};

cst yacc_token_tree_gen(token_tree* main_tt);