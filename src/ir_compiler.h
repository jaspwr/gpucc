#pragma once

#include <string>

#include "parse_tree.h"
#include "utils.h"

std::string serialize_uir_to_readable(void* ir, u32 ir_len, ParseTree& ir_tokens);