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
#include <glad/glad.h>

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
