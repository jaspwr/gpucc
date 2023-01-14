#include "debug_printing.h"

#include "shader_structures.h"

#include <iostream>
#include <math.h>

std::string check_both_parse_trees(GLuint token, ParseTree* lang_tokens, 
    ParseTree* abstract_tokens) {
        
    if (token == 1) return "Literal";
    if (token == 2) return "Identifier";
    if (abstract_tokens != nullptr) {
        auto abstract_tok = abstract_tokens->from_id(token);
        if (abstract_tok.length != 0) {
            return uintstring_to_string(abstract_tok);
        }
    }
    if (lang_tokens != nullptr) {
        auto lang_tok = lang_tokens->from_id(token);
        if (lang_tok.length != 0) {
            return uintstring_to_string(lang_tok);
        }
    }
    return "Unknown token";
}

void print_tokens(void* tokens, u32 length, 
    ParseTree& lang_tokens, ParseTree& abstract_tokens) {

    Token* _tokens = (Token*)tokens;
    for (u32 i = 0; i < length / sizeof(Token); i++) {
        auto token_id = _tokens[i].id;
        if (token_id == 0) continue;
        auto token_str = check_both_parse_trees(token_id, &lang_tokens, &abstract_tokens);
        i32 spacing = 15 - (i32)token_str.length() - std::to_string(i).length();
        if (spacing < 1) {
            spacing = 1;
        }
        std::cout << "[" << i << "] \"" << token_str << "\"" << repeated_char(spacing, ' ');
        _tokens[i].print();
    }
}

void print_ast_nodes(void* nodes, u32 length) {
    AstNode* _nodes = (AstNode*)_nodes;
    for (u32 i = 0; i < floor(length / sizeof(AstNode)); i++) {
        //if (_nodes[i].nodeToken == 0) continue;
        printf("[%d] ", i);
        _nodes[i].print();
    }
}