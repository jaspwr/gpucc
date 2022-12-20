#include "shader.h"

#include <vector>
#include <string>
#include <string.h>


#include "utils.h"


Shader::Shader(const char* shader_source_path, GLuint _barrier_mode) {
    barrier_mode = _barrier_mode;

    shader_index = glCreateShader(GL_COMPUTE_SHADER);
    auto path = get_bin_dir() + std::string("\\") + std::string(shader_source_path);
    const char* shader_source = load_file(path.c_str());
    glShaderSource(shader_index, 1, &shader_source, NULL);
    glCompileShader(shader_index);
    GLint isCompiled = 0;
    glGetShaderiv(shader_index, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader_index, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader_index, maxLength, &maxLength, &errorLog[0]);
        for(int i = 0; i < maxLength; i ++)
            printf("%c",(char)errorLog[i]);
        
        glDeleteShader(shader_index);
        throw (std::string("Failed to compile ") + std::string(shader_source_path)).c_str();
    }
    shader_bin_index = glCreateProgram();
    glAttachShader(shader_bin_index, shader_index);
    glLinkProgram(shader_bin_index);
    glFinish();
}

Shader::~Shader() {
    glDeleteProgram(shader_bin_index);
    glDeleteShader(shader_index);
}

void Shader::exec(u32 work_groups_x) {
    glFinish();

    if (work_groups_x == 0) throw "Work group count not set.";
    if (work_groups_x > GL_MAX_COMPUTE_WORK_GROUP_COUNT) throw "Tried to dispatch too many work groups.";
    glUseProgram(shader_bin_index);
    glDispatchCompute(work_groups_x, 1, 1);
    glMemoryBarrier(barrier_mode);
    glFinish();
}