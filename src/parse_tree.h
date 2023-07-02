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

#include <vector>

#include "gl.h"
#include "ssbo.h"

struct UintString {
    GLuint* data;
    GLuint length;
};

#include "utils.h"

struct ParseTreeEntry {
    UintString matches;
    GLuint points_to;
};

struct ParseTreeItem {
    GLuint next_row = 0;
    GLuint final_pointer = 0;
};

#define COLUMNS 256

class ParseTree {
    private:
        ParseTreeItem* tree;
        GLuint rows;
        u32 max_rows;
        std::vector<ParseTreeEntry>* entries;
    public:
        u32 size;
        ParseTree(std::vector<ParseTreeEntry> entries);
        ParseTree(u32 size_for_non_fixed);
        Ssbo* into_ssbo();
        GLuint exec(UintString input);
        UintString from_id(GLuint id);
        void append_entry(ParseTreeEntry entry);
        std::string to_shader_defs();
        ~ParseTree();
};

GLuint get_token_id(ParseTree& parse_tree, char* name, GLuint& last);
GLuint get_token_id(ParseTree& parse_tree, char* name);
