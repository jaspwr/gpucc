#pragma once
#include "yacc_parser.h"
#include "shader.h"

class lang
{
    public:
        inline static void regen_lang();
        inline static GLuint vram_token_tree;
        inline static GLuint vram_cst;

    private:
        inline static token_tree _token_tree;
        inline static cst _cst;
        lang() {}
};