#include "yacc.h"

#include <vector>

struct ast_parse_data {
    std::vector<GLuint> match;
    std::vector<GLuint> pre_exclusions;
    std::vector<GLuint> post_exclusions;
    GLuint signigicant_toknes[8];
};

#define AST_BUFFER_SIZE 1000

void append(GLuint* ast_nodes_buffer, GLuint& ast_nodes_len, GLuint value) {
    ast_nodes_buffer[ast_nodes_len] = value;
    ast_nodes_len++;
}

void flush_ast_parse_data(ast_parse_data& data) {
    data.pre_exclusions.clear();
    data.post_exclusions.clear();
    data.match.clear();
    memset(data.signigicant_toknes, 0, 8 * sizeof(GLuint));
}

GLuint append_ast_parse_data( ast_parse_data& data, GLuint* ast_nodes_buffer,
    GLuint& ast_nodes_len, ParseTree& ast_parse_tree, GLuint node_token) {
    auto pre_len = data.pre_exclusions.size();
    auto start_index = ast_nodes_len;

    // ast_nodes structure:
    // 0: node_token
    // 1: pre_exclusions_len
    // 2->len: pre_exclusions
    // 3: post_exclusions_len
    // 4->len: post_exclusions
    // 5->x8: signigicant_toknes


    append(ast_nodes_buffer, ast_nodes_len, node_token);
    append(ast_nodes_buffer, ast_nodes_len, pre_len);
    for (GLuint i = 0; i < pre_len; i++) {
        append(ast_nodes_buffer, ast_nodes_len, data.pre_exclusions[i]);
    }
    auto post_len = data.post_exclusions.size();
    append(ast_nodes_buffer, ast_nodes_len, post_len);
    for (GLuint i = 0; i < post_len; i++) {
        append(ast_nodes_buffer, ast_nodes_len, data.post_exclusions[i]);
    }
    for (GLuint i = 0; i < 8; i++) {
        append(ast_nodes_buffer, ast_nodes_len, data.signigicant_toknes[i]);
    }

    auto str = to_uint_string(data.match);
    ast_parse_tree.append_entry({str, start_index});
    delete str.data;

    flush_ast_parse_data(data);

    return start_index;
}

enum YaccContext {
    BlockName,
    Line,
    PreExclusions,
    PostExclusions,
    SymmetricExclusions,
    Codegen,
};

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\0';
}

bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool is_numeric(char c) {
    return c >= '0' && c <= '9';
}

bool is_alphanumeric(char c) {
    return is_alpha(c) || is_numeric(c);
}

enum CharCatergory {
    Alphanumeric,
    Whitespace,
    Other,
};

u8 enumerate_char_category(char c) {
    if (is_alphanumeric(c))
        return CharCatergory::Alphanumeric;
    if (is_whitespace(c))
        return CharCatergory::Whitespace;
    return CharCatergory::Other;
}

char fetch_char(std::string& grammar, i32 i) {
    if (i < 0) {
        return '\0';
    }
    return grammar[i];
}

char* strip_quote_marks(char* str) {
    // Very safe, I promise.
    str++;
    str[strlen(str) - 1] = '\0';
    return str;
}

void append_to_string_buffer(char* token_buffer, i32& token_buffer_index, char c) {
    token_buffer[token_buffer_index] = c;
    token_buffer_index++;
    if (token_buffer_index >= TOKEN_BUFFER_SIZE) {
        throw "Token buffer overflow. Increase TOKEN_BUFFER_SIZE in yacc.h.";
    }
    token_buffer[token_buffer_index] = '\0';
}

void flush_string_buffer(char* token_buffer, i32& token_buffer_index) {
    token_buffer[0] = '\0';
    token_buffer_index = 0;
}

GLuint get_token_id(ParseTree& parse_tree, char* name, GLuint& last) {
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
    auto _name = to_uint_string(name);

    auto token = parse_tree.exec(_name);
    if (token == 0) {
        throw "Token not found.";
    }
    delete _name.data;
    return token;
}

void handle_token(char* token_buffer, YaccContext& context, ParseTree& parse_tree, ParseTree& ast_parse_tree,
    GLuint* ast_nodes_buffer, GLuint& ast_nodes_len, GLuint& block_token, 
    ParseTree& yacc_parse_tree, GLuint& yacc_tokens_last, ast_parse_data& ast_pt_data) {
    

    // TODO: Make this a loop or something.
    if(strcmp(token_buffer, ":") == 0) {
        if (context != YaccContext::BlockName) {
            throw "Unexpected colon. Did you forget a semicolon?";
        }
        context = YaccContext::Line;
        return;
    }
    if(strcmp(token_buffer, ";") == 0) {
        if (context != YaccContext::Line) {
            throw "Unexpected semicolon. Are there any unbalanced brackets?";
        }
        if (ast_pt_data.match.size() == 0) {
            throw "Empty match case.";
        }
        append_ast_parse_data(ast_pt_data, ast_nodes_buffer, ast_nodes_len, ast_parse_tree, block_token);
        context = YaccContext::BlockName;
        return;
    }
    if(strcmp(token_buffer, "[") == 0) {
        if (context != YaccContext::Line) {
            throw "Cannot open exclusions block here.";
        }
        context = YaccContext::PreExclusions;
        return;
    }
    if(strcmp(token_buffer, "]") == 0) {
        if (context != YaccContext::Line) {
            throw "Cannot open exclusions block here.";
        }
        context = YaccContext::PreExclusions;
        return;
    }
    if(strcmp(token_buffer, "{") == 0) {
        if (context != YaccContext::Line) {
            throw "Cannot open exclusions block here.";
        }
        context = YaccContext::SymmetricExclusions;
        return;
    }
    if(strcmp(token_buffer, "}") == 0) {
        if (context != YaccContext::PreExclusions 
        && context != YaccContext::PostExclusions
        && context != YaccContext::SymmetricExclusions) {
            throw "Found '}', but no exclusions block was opened.";
        }
        context = YaccContext::Line;
        return;
    }
    if(strcmp(token_buffer, "<") == 0) {
        if (context != YaccContext::BlockName) {
            throw "Cannot open codegen block here.";
        }
        context = YaccContext::Codegen;
        return;
    }
    if(strcmp(token_buffer, ">") == 0) {
        if (context != YaccContext::Codegen) {
            throw "Found '>', but no codegen block was opened.";
        }
        context = YaccContext::BlockName;
        return;
    }

    if(strcmp(token_buffer, "|") == 0) {
        if (context != YaccContext::Line) {
            throw "Unexpected '|'.";
        }
        if (ast_pt_data.match.size() == 0) {
            throw "Empty match case.";
        }
        append_ast_parse_data(ast_pt_data, ast_nodes_buffer, ast_nodes_len, ast_parse_tree, block_token);
        return;
    }

    if (context == YaccContext::BlockName) {
        block_token = get_token_id(yacc_parse_tree, token_buffer, yacc_tokens_last);
        return;
    }

    if (context == YaccContext::Codegen) {
        throw "Codegen not implemented yet.";
        return;
    }

    GLuint token = token_buffer[0] == '\''
        ? get_token_id(parse_tree, strip_quote_marks(token_buffer))
        : get_token_id(yacc_parse_tree, token_buffer, yacc_tokens_last);

    if (context == YaccContext::Line) {
        ast_pt_data.match.push_back(token);
    } else if (context == YaccContext::PreExclusions) {
        ast_pt_data.pre_exclusions.push_back(token);
    } else if (context == YaccContext::PostExclusions) {
        ast_pt_data.post_exclusions.push_back(token);
    } else if (context == YaccContext::SymmetricExclusions) {
        ast_pt_data.pre_exclusions.push_back(token);
        ast_pt_data.post_exclusions.push_back(token);
    }


}

void parse_yacc(ParseTree& parse_tree, ParseTree& ast_parse_tree, std::string grammar,
    GLuint* ast_nodes_buffer, GLuint& ast_nodes_len) {
    char token_buffer[TOKEN_BUFFER_SIZE];
    token_buffer[0] = '\0';
    i32 token_buffer_index = 0;
    auto context = YaccContext::BlockName;
    bool in_string = false;
    GLuint block_token = 0;
    GLuint yacc_tokens_last = 90;
    ParseTree yacc_parse_tree = ParseTree({}, true);
    ast_parse_data ast_pt_data = ast_parse_data();
    // This has this weird previous character thing because it needs to deal with the final token
    for (i32 i = 0; fetch_char(grammar, i - 1) != '\0' || i == 0; i++) {
        auto c_cur = fetch_char(grammar, i);
        auto c_pre = fetch_char(grammar, i - 1);
        auto cur_cat = enumerate_char_category(c_cur);
        auto pre_cat = enumerate_char_category(c_pre);

        if (!in_string && (cur_cat != pre_cat || pre_cat == 2)) {
            if (pre_cat != CharCatergory::Whitespace) {
                handle_token( token_buffer, context, parse_tree, ast_parse_tree, ast_nodes_buffer,
                              ast_nodes_len, block_token, yacc_parse_tree, yacc_tokens_last, 
                              ast_pt_data);
            }
            flush_string_buffer(token_buffer, token_buffer_index);
        }
        if (c_cur == '\'') {
            in_string = !in_string;
        }
        append_to_string_buffer(token_buffer, token_buffer_index, c_cur);
    }
}

ast_ssbos create_ast_ssbos(std::string grammar, ParseTree& lang_tokens_parse_tree) {
    auto ssbos = ast_ssbos();
    auto ast_parse_tree = ParseTree({}, true);
    GLuint ast_nodes_len = 1;
    GLuint ast_nodes_buffer[AST_BUFFER_SIZE];
    parse_yacc(lang_tokens_parse_tree, ast_parse_tree, grammar, ast_nodes_buffer, ast_nodes_len);
    ssbos.ast_parse_tree = ast_parse_tree.into_ssbo();
    ssbos.ast_nodes = new Ssbo(ast_nodes_len * sizeof(GLuint), ast_nodes_buffer);
    return ssbos;
}

void delete_ast_ssbos(ast_ssbos ssbos) {
    delete ssbos.ast_parse_tree;
    delete ssbos.ast_nodes;
}