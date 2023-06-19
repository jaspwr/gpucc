#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "parse_tree.h"
#include "utils.h"

struct IrToken {
    std::string name;
    GLuint id;
};

typedef std::vector<IrToken> IrTokenList;

std::string serialize_uir_to_readable(const GLuint* ir, u32 ir_len,
    IrTokenList& ir_tokens, std::string& source);

std::string ir_tokens_shader_definitions(IrTokenList& ir_tokens);