#pragma once

#include "ssbo.h"
#include "types.h"
#include "parse_tree.h"

struct InstSelRet {
    Ssbo* match_parse_tree;
    Ssbo* type_checking;
    Ssbo* replacements;
};

InstSelRet parse_instruction_selection(const char* schema, ParseTree& ir_tokens);