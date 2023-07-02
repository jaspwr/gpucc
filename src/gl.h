// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of Meow.
//
// Meow is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// Meow is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Meow. If not, see <https://www.gnu.org/licenses/>.


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
    Shader liveness;
    Shader register_allocator;
};

class Gl {
    public:
        static void init(bool dbg_output);
        static Shaders compile_shaders();
};
