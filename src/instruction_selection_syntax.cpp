#include "instruction_selection_syntax.h"

#include "utils.h"
#include "exception.h"

#include <string>
#include <vector>
#include <iostream>

enum class SchemaCtx: u32 {
    Match,
    ActionLHS,
    ActionRHS,
    Replacement
};

void action(std::vector<std::string>& action, std::string& type_) {
    
}

void new_match(std::vector<std::string>& match, std::vector<std::string>& action, std::string& type_) {
    for (auto token : match) {
        std::cout << token << " ";
    }
    std::cout << "-> ";
    std::cout << type_ << " := ";
    for (auto token : action) {
        std::cout << token << " ";
    }
    std::cout << std::endl;
}

inline void handle_token(std::vector<std::string>& match,
    std::vector<std::string>& tokens, std::string& type_,
    SchemaCtx& ctx, std::string& token) {

    if (ctx == SchemaCtx::Match && token == "->") {
        ctx = SchemaCtx::ActionLHS;
        match = tokens;
        tokens = std::vector<std::string>();
    } else if (ctx == SchemaCtx::ActionLHS && token == ":=") {
        ctx = SchemaCtx::ActionRHS;

        if (tokens.size() != 1) {
            throw Exception("Type assign action RHS has too many or no tokens");
        }
        type_ = tokens[0];
        tokens = std::vector<std::string>();
    } else if (ctx == SchemaCtx::ActionRHS && token == "|") {
        // ignored
    } else if (ctx == SchemaCtx::ActionRHS && token == ";") {
        new_match(match, tokens, type_);
        ctx = SchemaCtx::Match;
        tokens = std::vector<std::string>();
    } else {
        tokens.push_back(token);
    }
}

TypePropagationRet parse_type_propagation(const char* schema) {

    auto token = std::string();
    auto tokens = std::vector<std::string>();

    auto type_ = std::string();

    auto match = std::vector<std::string>();

    auto ctx = SchemaCtx::Match;
    for (u32 i = 0; schema[i] != '\0'; i++) {
        char c = schema[i];
        if (char_utils::is_whitespace(c)) {
            if (token.length() > 0) {
                handle_token(match, tokens, type_, ctx, token);
                token = std::string();
            }
        } else {
            token += c;
        }
    }

    auto ret = TypePropagationRet();

    return ret;
}