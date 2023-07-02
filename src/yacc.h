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

#include "ssbo.h"
#include "parse_tree.h"
#include "ir_compiler.h"
#include "types.h"

#include <string>

#define TOKEN_BUFFER_SIZE 500

struct ast_ssbos {
    std::vector<Ssbo*> ast_parse_trees;
    Ssbo* ast_nodes;
    Ssbo* ir_codegen;
};

ast_ssbos create_ast_ssbos(std::vector<std::string> grammars, ParseTree& lang_tokens_parse_tree,
    IrTokenList* ir_token_list, ParseTree& yacc_parse_tree, ParseTree& ir_pt);
void delete_ast_ssbos(ast_ssbos ssbos);
