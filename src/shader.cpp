// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of gpucc.
//
// gpucc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// gpucc is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gpucc. If not, see <https://www.gnu.org/licenses/>.


#include "shader.h"
#include "exception.h"
#include "preprocessor.h"

#include <vector>
#include <string>
#include <string.h>

#include "shader_preproc.h"
#include "utils.h"


Shader::Shader(const char* shader_source_path, GLuint _barrier_mode) {
    barrier_mode = _barrier_mode;

    shader_index = glCreateShader(GL_COMPUTE_SHADER);
    auto path_of_raw = get_bin_dir() + std::string("/") + std::string(shader_source_path);
    std::string path = path_of_raw + ".out";

    try {
        preproc(path_of_raw.c_str(), path.c_str());
    } catch (std::string e) {
        throw Exception(e);
    }


    // auto var_reg = VariableRegistry();
    // auto s = preprocess(path, var_reg);
    // const char* shader_source = s.c_str();
    // printf("%s:\n%s\n", path.c_str(), shader_source);

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
        throw Exception(std::string("Failed to compile ") + std::string(shader_source_path));
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

    if (work_groups_x == 0) throw Exception("Invalid work group count. Can not be 0.");
    if (work_groups_x > GL_MAX_COMPUTE_WORK_GROUP_COUNT) throw Exception("Tried to dispatch too many work groups.");
    glUseProgram(shader_bin_index);
    glDispatchCompute(work_groups_x, 1, 1);
    glMemoryBarrier(barrier_mode);
    glFinish();
}
