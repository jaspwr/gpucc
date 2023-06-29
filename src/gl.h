#pragma once
#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"

#include <stdio.h>
#include <vector>

#include "shader.h"

#define AST_INVOCATIONS 32
#define AST_CHARS_PER_INV 4

#define CODEGEN_INVOCATIONS 32

#define TOKENISER_INVOCATIONS 32
#define TOKENISER_CHARS_PER_INV 4

struct Shaders {
    Shader tokeniser;
    Shader literals;
    Shader ast;
    Shader codegen;
    Shader type_propagation;
    Shader instruction_selection;
};

class Gl {
    public:
        static void init(bool dbg_output);
        static Shaders compile_shaders();
};