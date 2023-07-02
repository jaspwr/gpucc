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
#include "utils.h"
#include "parse_tree.h"

#define FIRST_TOKEN_ID 4

ParseTree* create_token_parse_tree(char** tokens, u32 token_count, u32 first_token_id);
ParseTree* tokens_list_to_parse_tree(const char* token_list);
std::string shader_token_defs(const char* token_list);
