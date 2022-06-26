#pragma once
#include "yacc_parser.h"
#include "shader.h"

class lang
{
    public:
        inline static void regen_lang();
        inline static GLuint vram_token_tree;
        inline static GLuint vram_cst;
        inline static void load_lang(char* preset);

    private:
        inline static token_tree _token_tree;
        inline static token_tree _cst;
        lang() {}
};