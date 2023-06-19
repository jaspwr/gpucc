#pragma once

#include "ssbo.h"
#include "types.h"

struct TypePropagationRet {
    Ssbo* match_parse_tree;
    Ssbo* type_set_action;
};

TypePropagationRet parse_type_propagation(const char* schema);