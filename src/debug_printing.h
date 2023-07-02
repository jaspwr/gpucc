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
