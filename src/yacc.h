#pragma once

#include "ssbo.h"
#include "parse_tree.h"

#include <string>

#define TOKEN_BUFFER_SIZE 500

struct ast_ssbos {
    Ssbo* ast_parse_tree;
    Ssbo* ast_nodes;
};

ast_ssbos create_ast_ssbos(std::string grammar, ParseTree& lang_tokens_parse_tree);
void delete_ast_ssbos(ast_ssbos ssbos);