#pragma once

#include <string>
#include <vector>

#include "parse_tree.h"
#include "utils.h"

struct IrToken {
    std::string name;
    GLuint id;
};

typedef std::vector<IrToken> IrTokenList;

std::string serialize_uir_to_readable(GLuint* ir, u32 ir_len, IrTokenList& ir_tokens);