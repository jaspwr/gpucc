#pragma once

#include "gl.h"
#include "parse_tree.h"
#include "ir_compiler.h"

#include <string>
#include <vector>

void append_codegen_ssbo_entry(GLuint* codegen_ssbo, GLuint& codegen_ssbo_len,
GLuint node_token, std::vector<std::string>& ir, ParseTree& ir_pt, IrTokenList* ir_tokens);

// Reserved IR tokens
#define IR_REFERNCE 1
#define IR_SELF_REFERENCE 2
#define IR_INSERSION 3
#define IR_SOURCE_POS_REF 4
#define IR_LITERAL_REF 5

#define IR_OTHER_TOKENS_START 6