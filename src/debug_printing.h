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
void print_ast_nodes(void* nodes, u32 length, ParseTree& lang_tokens, ParseTree& abstract_tokens);
void print_types(void* types, u32 length);
void print_asm(void* asm_data, u32 asm_length, void* phys_reg_map_data, u32 phys_reg_map_length);
void print_live_intervals(void* data, u32 length);
