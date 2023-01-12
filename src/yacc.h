#pragma once

#include "ssbo.h"
#include "parse_tree.h"
#include "ir_compiler.h"

#include <string>

#define TOKEN_BUFFER_SIZE 500

struct ast_ssbos {
    std::vector<Ssbo*> ast_parse_trees;
    Ssbo* ast_nodes;
    Ssbo* ir_codegen;
};

ast_ssbos create_ast_ssbos(std::vector<std::string> grammars, ParseTree& lang_tokens_parse_tree, IrTokenList* ir_token_list, ParseTree& yacc_parse_tree);
void delete_ast_ssbos(ast_ssbos ssbos);