#pragma once

#include "inc.h"
#include "gl.h"

enum shader_binding_type{
    image,
    ssbo,
    none
};

struct shader_binding{
    public:
        GLuint texture;
        GLenum access;
        GLenum format;
        shader_binding_type type;
};

class shader{
    public:
        GLuint index;
        GLuint program_index;
        shader_binding *shader_bindings; 
        void exec(GLuint dsp_x, GLuint dsp_y, GLuint dsp_z, bool rebind);
        shader(shader_binding *_shader_bindings, int _binds_count, const char **shader_source);
    private:
        int binds_count;

};
