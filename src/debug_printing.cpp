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


#include "debug_printing.h"

#include "shader_structures.h"
#include "cli.h"
#include "ir_types.h"

#include "arch/x86_64/opcodes.h"
#include "arch/x86_64/registers.h"

#include <iostream>
#include <math.h>
#include <unordered_map>

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

AstNode get_ast_node(GLuint* ast_nodes, u32 index) {

    #define AST_NODE_SIZE 11

    return {
        ast_nodes[index * AST_NODE_SIZE + 0],
        (int) ast_nodes[index * AST_NODE_SIZE + 1],
        ast_nodes[index * AST_NODE_SIZE + 2],
        (int) ast_nodes[index * AST_NODE_SIZE + 3],
        ast_nodes[index * AST_NODE_SIZE + 4],
        (int) ast_nodes[index * AST_NODE_SIZE + 5],
        ast_nodes[index * AST_NODE_SIZE + 6],
        (int) ast_nodes[index * AST_NODE_SIZE + 7],
        ast_nodes[index * AST_NODE_SIZE + 8],
        ast_nodes[index * AST_NODE_SIZE + 9],
        ast_nodes[index * AST_NODE_SIZE + 10]
    };
}

inline std::vector<AstNode> get_ast_nodes(GLuint* _nodes, u32 length) {
    std::vector<AstNode> ast_nodes;
    for (u32 i = 0; i < floor(length / sizeof(AstNode)); i++) {
        AstNode node = get_ast_node(_nodes, i);
        if (node.nodeToken == 0) continue;
        ast_nodes.push_back(node);
    }
    return ast_nodes;
}

void print_live_intervals(void* data, u32 length) {
    LiveInterval* intervals = (LiveInterval*)data;
    for (u32 i = 0; i < length / sizeof(LiveInterval); i++) {
        if (intervals[i].start == 0 && intervals[i].end == 0) continue;
        std::cout << "[%" << i << "] ";
        std::cout << "(" << intervals[i].start << " -> ";
        std::cout << intervals[i].end << ")";
        std::cout << "\n";
    }
}

void print_asm(void* asm_data, u32 asm_length, void* phys_reg_map_data,
               u32 phys_reg_map_length, void* ast_data, u32 ast_length,
               void* frame_pointer_data, u32 frame_pointer_length) {

    GLint* asm_tokens = (GLint*)asm_data;
    GLuint* phys_reg_map = (GLuint*)phys_reg_map_data;
    GLuint* frame_pointers = (GLuint*)frame_pointer_data;

    bool last_opcode = false;

    for (u32 i = 0; i < asm_length / sizeof(GLuint); i++) {
        GLint token = asm_tokens[i];
        if (token == 0) continue;
        if (token > 0) {
            last_opcode = true;
            std::cout << "\n";
            // std::cout << i << ": ";
            std::cout << from_opcode(token);
        } else {
            if (last_opcode) {
                last_opcode = false;
            } else {
                std::cout << ",";
            }

            GLuint vreg = -token;
            GLuint phys_reg = phys_reg_map[vreg];
            if (phys_reg == 0) {
                AstNode node = get_ast_node((GLuint*)ast_data, vreg - 1);
                if (node.nodeToken == 1 /* Literals */) { // TODO: Do properly.
                    u64 value = node.children[0].codegenVolume;
                    value <<= 32;
                    value |= node.children[1].codegenVolume;
                    printf(" 0x%llx", value);
                } else if (frame_pointers[vreg] != 0) {
                    // TODO: pointer lengths.
                    std::cout << " DWORD [rbp - " << frame_pointers[vreg] << "]";
                } else {
                    std::cout << " %" << vreg;
                }
            } else {
                // std::cout << " " << from_register(phys_reg) << " (%" << vreg << ")";
                std::cout << " " << from_register(phys_reg);
            }
        }

    }
    std::cout << "\n";
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

void print_centered(std::string str, u32 width) {
    u32 spacing = width - str.length();
    if (spacing < 1) {
        spacing = 1;
    }
    std::cout << repeated_char(spacing / 2, ' ') << str << repeated_char(spacing / 2, ' ');
}


void print_types(void* types, u32 length) {
    #define TYPE_STRUCT_SIZE 3
    GLuint* _types = (GLuint*)types;
    for (u32 i = 0; i < length / sizeof(GLuint); i += TYPE_STRUCT_SIZE) {
        if (_types[i + 0] == 0) continue;
        std::cout << "[%" << i / TYPE_STRUCT_SIZE << "] ";
        std::cout << "Type: " << ir_types::id_to_string(_types[i + 0]) << " ";
        std::cout << "PointerDepth: " << _types[i + 1] << " ";
        std::cout << "qualifiers: " << _types[i + 2] << " ";
        std::cout << "\n";
    }
}

void print_tree_lines(u32 nodes_count, u32 width) {
    bool even = nodes_count % 2 == 0;
    std::vector<u32> down_node_positions;
    auto seg_size = width / nodes_count;
    auto center = width / 2;
    for (u32 i = 0; i < nodes_count; i++) {
        down_node_positions.push_back(seg_size * i + seg_size / 2);
    }
    if (nodes_count == 1) {
        print_centered("┃", width);
    }
    for (u32 i = 0; i < width; i++) {
        if (i < down_node_positions[0] || i > down_node_positions[down_node_positions.size() - 1]) {
            std::cout << " ";
            continue;
        }
        if (i == center) {
            if (even) {
                std::cout << "┻";
            } else {
                std::cout << "╋";
            }
            continue;
        }
        if (i == down_node_positions[0]) {
            std::cout << "┏";
            continue;
        }
        if (i == down_node_positions[down_node_positions.size() - 1]) {
            std::cout << "┓";
            continue;
        }
        bool printed_flag = false;
        for (u32 j = 0; j < down_node_positions.size(); j++) {
            if (i == down_node_positions[j]) {
                std::cout << "┳";
                printed_flag = true;
                break;
            }
        }
        if (printed_flag) continue;
        std::cout << "━";
    }
}

void print_ast_nodes(void* nodes, u32 length, ParseTree& lang_tokens, ParseTree& abstract_tokens) {
    GLuint* _nodes = (GLuint*)nodes;
    std::vector<AstNode> ast_nodes = get_ast_nodes(_nodes, length);
    AstNode& entry = ast_nodes[0];
    GLuint largest_volume = 0;
    for (auto& node : ast_nodes) {
        if (node.volume > largest_volume) {
            largest_volume = node.volume;
            entry = node;
        }
        node.print(check_both_parse_trees(node.nodeToken, &lang_tokens, &abstract_tokens));

    }

    auto term_size = get_terminal_size();
    // print_centered("AST", term_size.cols);
    // printf("\n");
    // print_tree_lines(2, term_size.cols);
    // printf("\n");
    // print_centered("AST", term_size.cols / 2);
    // print_centered("AST", term_size.cols / 2);
    // printf("\n");

    entry.print(check_both_parse_trees(entry.nodeToken, &lang_tokens, &abstract_tokens));
    //print_node(entry, term_size.cols, ast_nodes, node_map, lang_tokens, abstract_tokens);
}



//┏━━╋━┻━┓
