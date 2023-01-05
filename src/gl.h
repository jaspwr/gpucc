#pragma once
#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"

#include <stdio.h>
#include <vector>

#include "shader.h"

struct Shaders {
    Shader tokeniser;
    Shader ast;
    Shader codegen;
};

class Gl {
    public:
        static void init();
        static Shaders compile_shaders();
};