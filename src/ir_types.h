#pragma once

#include "types.h"
#include "gl.h"

#include <string>

namespace ir_types {
    std::string shader_defs();
    std::string id_to_string(GLuint id);
    GLuint string_to_id(std::string str);
}
