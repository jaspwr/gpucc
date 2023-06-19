#pragma once

#include "job.h"
#include "gl.h"
#include "parse_tree.h"
#include "yacc.h"
#include "ir_compiler.h"

std::string compile(Job& job, Shaders& shaders, ParseTree& yacc_parse_tree,
                    ParseTree& ir_parse_tree, ParseTree* lang_tokens_parse_tree,
                    IrTokenList* ir_tokens, ast_ssbos _ast_ssbos);

