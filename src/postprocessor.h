#pragma once

#include "gl.h"
#include "variable_registry.h"
#include "parse_tree.h"

struct SizedGLintBuffer {
    ~SizedGLintBuffer();

    GLint* data;
    u32 size;
};

SizedGLintBuffer postprocess(const GLint* shader_out, GLuint shader_out_size,
    VariableRegistry& var_reg, ParseTree& ir_tokens, std::string& source);