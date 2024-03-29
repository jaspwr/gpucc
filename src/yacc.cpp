// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of gpucc.
//
// gpucc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// gpucc is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gpucc. If not, see <https://www.gnu.org/licenses/>.


#include "yacc.h"
#include "exception.h"

#include "ir_compiler.h"
#include "ir_ssbo_format.h"

#include <vector>
#include <string.h>

struct AstParseData {
    std::vector<GLuint> match;
    std::vector<GLuint> pre_exclusions;
    std::vector<GLuint> post_exclusions;
    std::vector<std::string> codegen;
    GLuint signigicant_toknes[4];
};

#define AST_BUFFER_SIZE 10000
#define IR_CODEGEN_BUFFER_SIZE 10000

void append(GLuint* ast_nodes_buffer, GLuint& ast_nodes_len, GLuint value) {
    ast_nodes_buffer[ast_nodes_len] = value;
    ast_nodes_len++;
}

void flush_ast_parse_data(AstParseData& data) {
    data.pre_exclusions.clear();
    data.post_exclusions.clear();
    data.match.clear();
    memset(data.signigicant_toknes, 0, 4 * sizeof(GLuint));
}

GLuint append_ast_parse_data( AstParseData& data, GLuint* ast_nodes_buffer,
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
    for (GLuint i = 0; i < 4; i++) {
        append(ast_nodes_buffer, ast_nodes_len, data.signigicant_toknes[i]);
    }

    auto str = to_uint_string(data.match);
    ast_parse_tree.append_entry({str, start_index});
    delete str.data;

    flush_ast_parse_data(data);

    return start_index;
}

enum class YaccContext: u32 {
    BlockName,
    Line,
    PreExclusions,
    PostExclusions,
    SymmetricExclusions,
    Codegen,
    TypeAction,
};

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\0';
}

bool is_alpha(char c) {
    return char_utils::is_alpha(c) || c == '_' || c == '#';
}

bool is_numeric(char c) {
    return char_utils::is_numeric(c) || c == '$' || c == '!'; // also includes prefixes
}

bool is_alphanumeric(char c) {
    return is_alpha(c) || is_numeric(c);
}

enum class CharCatergory: u32 {
    Alphanumeric,
    Whitespace,
    Other,
};

CharCatergory enumerate_char_category(char c) {
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
        throw Exception("Token buffer overflow. Increase TOKEN_BUFFER_SIZE in yacc.h.");
    }
    token_buffer[token_buffer_index] = '\0';
}

void flush_string_buffer(char* token_buffer, i32& token_buffer_index) {
    token_buffer[0] = '\0';
    token_buffer_index = 0;
}

u32 parse_prefixed_number (char* str) {
    u32 n = 0;
    str++;
    while (*str >= '0' && *str <= '9') {
        n = n * 10 + (*str - '0');
        str++;
    }
    return n;
}

GLuint yacc_constant(char* token) {
    if (strcmp(token, "#identifier") == 0) return 2;
    if (strcmp(token, "#literal") == 0) return 1;
    throw Exception(std::string("Invalid constant \"") + std::string(token) + std::string("\"."));
}

void handle_token(char* token_buffer, YaccContext& context, ParseTree& parse_tree, ParseTree& ast_parse_tree,
    GLuint* ast_nodes_buffer, GLuint& ast_nodes_len, GLuint& block_token, ParseTree& yacc_parse_tree,
    GLuint& yacc_tokens_last, AstParseData& ast_pt_data, GLuint* codegen_ssbo, GLuint& codegen_ssbo_len,
    ParseTree& ir_pt, IrTokenList* ir_tokens, std::vector<std::string>& type_actions) {

    if (context == YaccContext::TypeAction) {
        if (strcmp(token_buffer, "`") == 0) {
            context = YaccContext::Codegen;
        } else {
            type_actions.push_back(token_buffer);
            printf("Type action: %s\n", token_buffer);
        }
        return;
    }

    if (context == YaccContext::Codegen) {
        if (strcmp(token_buffer, "`") == 0) {
            context = YaccContext::TypeAction;
            return;
        }
        if (strcmp(token_buffer, ">") == 0) {
            context = YaccContext::BlockName;
            append_codegen_ssbo_entry(codegen_ssbo, codegen_ssbo_len, block_token,
                                      ast_pt_data.codegen, ir_pt, ir_tokens, type_actions);
            ast_pt_data.codegen.clear();
            type_actions = std::vector<std::string>();
        } else {
            ast_pt_data.codegen.push_back(token_buffer);
        }
        return;
    }
    if ((strcmp(token_buffer, "\n") == 0)) return;

    // TODO: Make this a loop or something.
    if (strcmp(token_buffer, ":") == 0) {
        if (context != YaccContext::BlockName) {
            throw Exception("Unexpected colon. Did you forget a semicolon?");
        }
        context = YaccContext::Line;
        return;
    }
    if (strcmp(token_buffer, ";") == 0) {
        if (context != YaccContext::Line) {
            throw Exception("Unexpected semicolon. Are there any unbalanced brackets?");
        }
        if (ast_pt_data.match.size() == 0) {
            throw Exception("Empty match case.");
        }
        append_ast_parse_data(ast_pt_data, ast_nodes_buffer, ast_nodes_len, ast_parse_tree, block_token);
        context = YaccContext::BlockName;
        return;
    }
    if (strcmp(token_buffer, "[") == 0) {
        if (context != YaccContext::Line) {
            throw Exception("Cannot open exclusions block here.");
        }
        context = YaccContext::PostExclusions;
        return;
    }
    if (strcmp(token_buffer, "]") == 0) {
        if (context != YaccContext::Line) {
            throw Exception("Cannot open exclusions block here.");
        }
        context = YaccContext::PreExclusions;
        return;
    }
    if (strcmp(token_buffer, "{") == 0) {
        if (context != YaccContext::Line) {
            throw Exception("Cannot open exclusions block here.");
        }
        context = YaccContext::SymmetricExclusions;
        return;
    }
    if (strcmp(token_buffer, "}") == 0) {
        if (context != YaccContext::PreExclusions
        && context != YaccContext::PostExclusions
        && context != YaccContext::SymmetricExclusions) {
            throw Exception("Found '}', but no exclusions block was opened.");
        }
        context = YaccContext::Line;
        return;
    }
    if (strcmp(token_buffer, "<") == 0) {
        if (context != YaccContext::BlockName) {
            throw Exception("Cannot open codegen block here.");
        }
        ast_pt_data.codegen.clear();
        context = YaccContext::Codegen;
        return;
    }

    if (strcmp(token_buffer, "|") == 0) {
        if (context != YaccContext::Line) {
            throw Exception("Unexpected '|'.");
        }
        if (ast_pt_data.match.size() == 0) {
            throw Exception("Empty match case.");
        }
        append_ast_parse_data(ast_pt_data, ast_nodes_buffer, ast_nodes_len, ast_parse_tree, block_token);
        return;
    }

    if (token_buffer[0] == '$') {
        if (context != YaccContext::Line) {
            throw Exception("Unexpected '$'.");
        }
        u32 index = parse_prefixed_number(token_buffer);
        ast_pt_data.signigicant_toknes[index] = ast_pt_data.match.size() + 1;
        return;
    }

    if (token_buffer[0] == '!') {
        throw Exception("Unexpected insertion. Insertions must be in codegen blocks.");
    }

    if (context == YaccContext::BlockName) {
        block_token = get_token_id(yacc_parse_tree, token_buffer, yacc_tokens_last);
        return;
    }

    GLuint token = 0;
    if (token_buffer[0] == ',') {
        return;
    }
    if (token_buffer[0] == '#') {
        token = yacc_constant(token_buffer);
    } else  {
        token = token_buffer[0] == '\''
            ? get_token_id(parse_tree, strip_quote_marks(token_buffer))
            : get_token_id(yacc_parse_tree, token_buffer, yacc_tokens_last);
    }

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

void parse_yacc(ParseTree& parse_tree, std::vector<Ssbo*>& ast_parse_trees, std::vector<std::string>& grammars,
    GLuint* ast_nodes_buffer, GLuint& ast_nodes_len, GLuint* ir_codegen, GLuint& ir_codegen_len, ParseTree& ir_pt,
    IrTokenList* ir_token_list, ParseTree& yacc_parse_tree) {
    char token_buffer[TOKEN_BUFFER_SIZE];
    token_buffer[0] = '\0';
    i32 token_buffer_index = 0;
    auto context = YaccContext::BlockName;
    bool in_string = false;
    GLuint block_token = 0;
    GLuint yacc_tokens_last = 90;
    AstParseData ast_pt_data = AstParseData();
    auto type_actions = std::vector<std::string>();

    for(auto grammar : grammars) {
        auto ast_parse_tree = ParseTree(600);
        // This has this weird previous character thing because it needs to deal with the final token
        for (i32 i = 0; fetch_char(grammar, i - 1) != '\0' || i == 0; i++) {
            auto c_cur = fetch_char(grammar, i);
            auto c_pre = fetch_char(grammar, i - 1);
            auto cur_cat = enumerate_char_category(c_cur);
            auto pre_cat = enumerate_char_category(c_pre);

            if (!in_string && (cur_cat != pre_cat || pre_cat == CharCatergory::Other)) {
                if (pre_cat != CharCatergory::Whitespace) {
                    handle_token( token_buffer, context, parse_tree, ast_parse_tree, ast_nodes_buffer,
                                ast_nodes_len, block_token, yacc_parse_tree, yacc_tokens_last,
                                ast_pt_data, ir_codegen, ir_codegen_len, ir_pt, ir_token_list, type_actions);
                }
                flush_string_buffer(token_buffer, token_buffer_index);
            }
            if (c_cur == '\'') {
                in_string = !in_string;
            }
            append_to_string_buffer(token_buffer, token_buffer_index, c_cur);
        }
        ast_parse_trees.push_back(ast_parse_tree.into_ssbo());
    }

}

ast_ssbos create_ast_ssbos(std::vector<std::string> grammars, ParseTree& lang_tokens_parse_tree,
    IrTokenList* ir_token_list, ParseTree& yacc_parse_tree, ParseTree& ir_pt) {

    auto ssbos = ast_ssbos();
    GLuint ast_nodes_len = 1;
    GLuint ast_nodes_buffer[AST_BUFFER_SIZE];
    GLuint ir_codegen[IR_CODEGEN_BUFFER_SIZE]; // TODO: Store this size somewhere and check for overflows
    memset(ir_codegen, 0, IR_CODEGEN_BUFFER_SIZE*sizeof(GLuint));
    GLuint ir_codegen_len = 256;

    try {
        parse_yacc(lang_tokens_parse_tree, ssbos.ast_parse_trees, grammars, ast_nodes_buffer, ast_nodes_len, ir_codegen,
            ir_codegen_len, ir_pt, ir_token_list, yacc_parse_tree);
    } catch (Exception& e) {
        // This just saves having to add `ExceptionType::Yacc` to every throw
        if (e.type == ExceptionType::Misc)
            e.type = ExceptionType::Yacc;
        throw e;
    }

    ssbos.ast_nodes = new Ssbo(ast_nodes_len * sizeof(GLuint), ast_nodes_buffer);
    ssbos.ir_codegen = new Ssbo(ir_codegen_len * sizeof(GLuint), ir_codegen);


    (void)ssbos.ast_nodes->print_size("AST nodes parse tree");
    (void)ssbos.ir_codegen->print_size("ir_codegen");


    return ssbos;

}

void delete_ast_ssbos(ast_ssbos ssbos) {
    for (auto ssbo : ssbos.ast_parse_trees) {
        delete ssbo;
    }
    delete ssbos.ast_nodes;
    delete ssbos.ir_codegen;
}
