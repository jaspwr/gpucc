#pragma once

#include <string>
#include "utils.h"

enum DebugPrintFn {
    None,
    Tokens,
    ASTNodes
};

void print_tokens(void* tokens, u32 length, 
    ParseTree& lang_tokens, ParseTree& abstract_tokens);
void print_ast_nodes(GLuint* nodes, u32 length,
    void* lang_tokens, void* abstract_tokens);