#pragma once

#include "inc.h"
#include "gl.h"

struct shader_binding{
    public:
        GLuint binding;
        GLuint texture;
        GLenum access;
        GLenum format;
};

class shader{
    public:
        GLuint index;
        GLuint program_index;
        shader_binding *shader_bindings;
        void init(shader_binding *_shader_bindings, const char **shader_source);
        void exec(GLuint dsp_x, GLuint dsp_y, GLuint dsp_z);

};
