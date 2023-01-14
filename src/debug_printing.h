#pragma once

#include <string>
#include "utils.h"
#include "types.h"

enum class DebugPrintFn: u32 {
    None,
    Tokens,
    ASTNodes
};

void print_tokens(void* tokens, u32 length, 
    ParseTree& lang_tokens, ParseTree& abstract_tokens);
void print_ast_nodes(void* nodes, u32 length);