#pragma once

#include "ssbo.h"
#include "utils.h"
#include "parse_tree.h"

#define FIRST_TOKEN_ID 4

ParseTree* create_token_parse_tree(char** tokens, u32 token_count, u32 first_token_id);
ParseTree* tokens_list_to_parse_tree(const char* token_list);
std::string shader_token_defs(const char* token_list);