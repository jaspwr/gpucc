#include "lang.h"

ParseTree* tokens_list_to_parse_tree(const char* token_list) {
    auto spl = str_split(token_list, '\n');
    auto ret = create_token_parse_tree(spl.spl, spl.len, 4);
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
    auto ret = new ParseTree(entries, false);
    // for (ParseTreeEntry entry : entries) {
    //     delete[] entry.matches.data;
    // }
    return ret;
}