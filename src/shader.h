#pragma once
#include "include/glad/glad.h"

#include "job.h"
#include "types.h"


class Shader {
    private:
        GLuint shader_bin_index;
        GLuint shader_index;
        GLuint barrier_mode; 
    public:
        Shader(const char* shader_source_path, GLuint _barrier_mode);
        Shader() {};
        ~Shader();
        void exec(u32 work_groups_x);
};

class Tokeniser: public Shader {};