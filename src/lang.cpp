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


#include "lang.h"

ParseTree* tokens_list_to_parse_tree(const char* token_list) {
    StrSplit spl = str_split(token_list, '\n');
    auto ret = create_token_parse_tree(spl.spl, spl.len, FIRST_TOKEN_ID);
    free_str_split(spl);
    return ret;
}

ParseTree* create_token_parse_tree(char** tokens, u32 token_count, u32 first_token_id) {
    auto entries = std::vector<ParseTreeEntry>();
    for (u32 i = 0; i < token_count; i++) {
        entries.push_back(ParseTreeEntry {
            to_uint_string(tokens[i]), (GLuint)(i + first_token_id)
        });
    }
    auto ret = new ParseTree(entries);
    for (ParseTreeEntry entry : entries) {
        delete[] entry.matches.data;
    }
    return ret;
}

std::string shader_token_defs(const char* token_list) {
    StrSplit tokens = str_split(token_list, '\n');
    auto ret = std::string();
    u32 id = FIRST_TOKEN_ID;
    for (u32 i = 0; i < tokens.len; i++) {
        auto token = tokens.spl[i];

        if (!char_utils::string_is_var_name(token)
            || char_utils::is_numeric(token[0])) {

            continue;
        }

        ret += "#define LANG_"
               + std::string(token)
               + " "
               + std::to_string(id++)
               + "\n";
    }
    return ret;
}
