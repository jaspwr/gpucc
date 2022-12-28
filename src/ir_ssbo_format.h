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
#define IR_INSERSION 2

#define IR_OTHER_TOKENS_START 3