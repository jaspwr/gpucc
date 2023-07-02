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

#include "gl.h"
#include "parse_tree.h"
#include "ir_compiler.h"

#include <string>
#include <vector>

void append_codegen_ssbo_entry(GLuint* codegen_ssbo, GLuint& codegen_ssbo_len,
    GLuint node_token, std::vector<std::string>& ir, ParseTree& ir_pt, IrTokenList* ir_tokens,
    std::vector<std::string>& type_actions);

// Reserved IR tokens
#define IR_REFERNCE 1
#define IR_SELF_REFERENCE 2
#define IR_INSERSION 3
#define IR_SOURCE_POS_REF 4
#define IR_LITERAL_REF 5

#define IR_OTHER_TOKENS_START 6
