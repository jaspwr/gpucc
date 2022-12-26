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
        ParseTree(std::vector<ParseTreeEntry> entries, bool non_fixed);
        Ssbo* into_ssbo();
        GLuint exec(UintString input);
        UintString from_id(GLuint id);
        void append_entry(ParseTreeEntry entry);
        ~ParseTree();
};