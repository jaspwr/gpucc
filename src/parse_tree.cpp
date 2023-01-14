#include "parse_tree.h"
#include "exception.h"

#include <string.h>

GLuint get_next_row(ParseTreeItem* tree, GLuint row, GLuint column) {
    return tree[row * COLUMNS + column].next_row;
}

void ParseTree::append_entry(ParseTreeEntry entry) {
    size++;
    GLuint row = 0;

    for (i32 i = 0; i < entry.matches.length - 1; i++)
    {
        GLuint c = entry.matches.data[i];
        auto next_row = get_next_row(tree, row, c);
        if (next_row == 0)
        {
            rows++;
            if (rows >= max_rows)
                throw Exception("Parse tree overflow.");
            tree[row * COLUMNS + c].next_row = rows;
            row = rows;
        } else
            row = next_row;
    }


    tree[row * COLUMNS + entry.matches.data[entry.matches.length - 1]]
        .final_pointer = entry.points_to;

    auto new_data = new GLuint[entry.matches.length];
    memcpy(new_data, entry.matches.data, entry.matches.length * sizeof(GLuint));
    entry.matches.data = new_data;
    entries->push_back(entry);
}

u32 sum_lens(std::vector<ParseTreeEntry>& entries) {
    u32 sum = 0;
    for (ParseTreeEntry entry : entries)
    {
        sum += entry.matches.length;
    }
    return sum;
}

UintString ParseTree::from_id(GLuint id) {
    for (ParseTreeEntry entry : *entries)
    {
        if (entry.points_to == id)
            return entry.matches;
    }
    return to_uint_string("");
}

ParseTree::ParseTree(std::vector<ParseTreeEntry> entries, bool non_fixed) {
    this->entries = new std::vector<ParseTreeEntry>();

    max_rows = non_fixed ? 700 : sum_lens(entries);
    tree = new ParseTreeItem[COLUMNS * max_rows + 40];
    rows = 0;
    size = 0;
    for (ParseTreeEntry entry : entries)
    {
        if (entry.matches.length == 0)
            throw Exception("Tried to create parse tree with empty string. Is there a blank line in your token list?");
        append_entry(entry);
    }
}

Ssbo* ParseTree::into_ssbo() {
    Ssbo* ssbo = new Ssbo((rows + 1) * COLUMNS * sizeof(ParseTreeItem), tree);
    return ssbo;
}

GLuint ParseTree::exec(UintString input) {
    GLuint row = 0;
    for (u32 i = 0; i < input.length - 1; i++)
    {
        GLuint c = input.data[i];
        auto next_row = get_next_row(tree, row, c);
        if (next_row == 0)
        {
            return 0;
        }
        row = next_row;
    }
    return tree[row * COLUMNS + input.data[input.length - 1]].final_pointer;
}

// TODO: These overloads should be renamed to separate functions.

GLuint get_token_id(ParseTree& parse_tree, char* name, GLuint& last) {
    // Appends if not known
    auto _name = to_uint_string(name);

    auto token = parse_tree.exec(_name);
    if (token == 0) {
        parse_tree.append_entry({_name, ++last});
        token = last;
    }
    delete _name.data;
    return token;
}

GLuint get_token_id(ParseTree& parse_tree, char* name) {
    // Throws in not known
    auto _name = to_uint_string(name);

    auto token = parse_tree.exec(_name);
    if (token == 0) {
        throw std::string("Token \"") + std::string(name) + std::string("\" not found.");
    }
    delete _name.data;
    return token;
}

bool unit_test_parse_tree_generator() {
    // Leaks a little
    ParseTreeEntry entry1 = {to_uint_string("meow"), 20};
    ParseTreeEntry entry2 = {to_uint_string("hello"), 99};
    ParseTree tree = ParseTree({entry1, entry2}, false);
    return tree.exec(to_uint_string("meow")) == 20
        && tree.exec(to_uint_string("hello")) == 99
        && tree.exec(to_uint_string("nothing")) == 0;
}

ParseTree::~ParseTree() {
    for (ParseTreeEntry entry : *entries)
        delete entry.matches.data;
    delete entries;
    delete[] tree;
}