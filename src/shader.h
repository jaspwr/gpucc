#pragma once
#include "include/glad/glad.h"

#include "job.h"

class Shader {
    private:
        GLuint shader_bin_index;
        GLuint shader_index;
        GLuint barrier_mode; 
    public:
        Shader(const char* shader_source_path, GLuint _barrier_mode);
        ~Shader();
        void exec(unsigned int work_groups_x);
};

class Tokeniser: public Shader {};