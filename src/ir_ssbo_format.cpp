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


#include "ir_ssbo_format.h"
#include "exception.h"

#include <iostream>

#include "shaders/type_actions.h"

void append_children_offsets(GLuint* codegen_ssbo, GLuint& start_pos, GLuint children_offsets[4]) {
    for (i32 i = 0; i < 4; i++) {
        codegen_ssbo[start_pos + i] = children_offsets[i];
    }
}

void append_ir_token(std::string token, ParseTree& ir_pt, GLuint* codegen_ssbo, GLuint& codegen_ssbo_len, IrTokenList* ir_tokens) {
    char* token_name = (char*)token.c_str();
    GLuint size = ir_pt.size + IR_OTHER_TOKENS_START;
    GLuint pre_size = size;
    auto a = get_token_id(ir_pt, token_name, size);
    if (a > pre_size)
        ir_tokens->push_back({std::string(token_name), a});

    codegen_ssbo[codegen_ssbo_len++] = a;
}

std::string next_token(std::vector<std::string>& ir, u32& i, u32& len) {
    i++;
    if (i >= len) throw Exception("Expected antoher IR token in codegen. Is there a malfomed reference?");
    return ir[i];
}

void append_numeral(std::string number_str, GLuint max_num, GLuint* codegen_ssbo, GLuint& codegen_ssbo_len) {
    GLuint num = 0;
    for (auto c : number_str) {
        if (c == '$') continue;
        if (c < '0' || c > '9') throw Exception("Expected a number in codegen reference.");
        num *= 10;
        num += c - '0';
    }
    if (num > max_num) throw Exception("Number was too large.");
    codegen_ssbo[codegen_ssbo_len++] = num;
}

std::vector<GLuint> create_action(std::vector<std::string>& action) {
    // 0 : len
    // 1 : SELF OR REF
    // 2 : vreg if is ref

    // 3 : STATIC OR REF
    // 4 : ref or

    auto ret = std::vector<GLuint>();

    ret.push_back(0);

    if (action.size() == 0) return ret;

    GLuint len = 0;

    if (action[0] == "$x" || action[0] == "$X") {
        ret.push_back(TYPE_ACTION_SET_SELF_VREG);
        ret.push_back(0);
        len += 2;
    } else {
        ret.push_back(TYPE_ACTION_SET_VREG);
        ret.push_back(parse_int((char *)action[0].c_str() + 1));
        len += 2;
    }

    if (action[1] != ":" || action[2] != "=")
        throw Exception("Expected cock and balls operator in type action.");

    for (u32 i = 3; i < action.size(); i += 2) {
        if (action[i][0] == '$') {
            ret.push_back(TYPE_ACTION_TYPE_OF_VREG);
            auto a = parse_int((char *)action[i].c_str() + 1);
            ret.push_back(a);
        } else {
            ret.push_back(TYPE_ACTION_STATIC_TYPE);
            if (action[i] == "i1") {
                ret.push_back(TYPE_ACTION_I1);
            }
            // TODO
        }
        len += 2;
    }

    ret[0] = len;

    for (auto c : ret) {
        std::cout << "UHHHH " << c << std::endl;
    }

    return ret;
}

void append_codegen_ssbo_entry(GLuint* codegen_ssbo, GLuint& codegen_ssbo_len,
    GLuint node_token, std::vector<std::string>& ir, ParseTree& ir_pt, IrTokenList* ir_tokens,
    std::vector<std::string>& type_actions) {

    // Codegen SSBO structure
    // 0 -> 256 : Pointers to IR section that is inserted for nodes with the token at that index
    // IR section:
    //     0 -> 4 : Child offsets
    //     4 : Length of IR
    //     5 -> 5 + length : IR tokens
    //     5 + length : Type actions length
    //     6 + length -> 6 + length + type_actions_length : Type actions

    if (ir.size() == 0) return; // May need to write the codegen_ssbo[node_token] to 0 for this case to be safe (not sure if it's needed should check)

    codegen_ssbo[node_token] = codegen_ssbo_len; // Set pointer to IR for that node token

    GLuint start_pos = codegen_ssbo_len;
    codegen_ssbo_len += 4;
    GLuint len_pos = codegen_ssbo_len++; // Set this empty spot to length of IR once known
    GLuint init_codegen_ssbo_len = codegen_ssbo_len;
    GLuint children_offsets[4] = {0, 0, 0, 0};
    u32 len = ir.size();
    GLuint current_block_size = 0;
    for (u32 i = 0; i < len; i++) {
        switch (ir[i][0])
        {
        case '$':
            current_block_size += 2;
            if (ir[i] == "$x" || ir[i] == "$X") {
                // Self ref
                codegen_ssbo[codegen_ssbo_len++] = IR_SELF_REFERENCE;
                codegen_ssbo[codegen_ssbo_len++] = 0;
                break;
            }
            codegen_ssbo[codegen_ssbo_len++] = IR_REFERNCE;
            append_numeral(ir[i], 3, codegen_ssbo, codegen_ssbo_len);
            break;
        case '!': {
            char* tok_str = (char*)ir[i].c_str();
            children_offsets[parse_int(tok_str + 1)] = current_block_size;
            // codegen_ssbo[codegen_ssbo_len++] = IR_INSERSION;
            // append_ir_token(ir[i], ir_pt, codegen_ssbo, codegen_ssbo_len, ir_tokens);
            break;
        }
        default:
            current_block_size++;
            append_ir_token(ir[i], ir_pt, codegen_ssbo, codegen_ssbo_len, ir_tokens);
            break;
        }
    }
    codegen_ssbo[len_pos] = codegen_ssbo_len - init_codegen_ssbo_len; // Set length of IR

    for (auto c : create_action(type_actions)) {
        codegen_ssbo[codegen_ssbo_len++] = c;
    }

    append_children_offsets(codegen_ssbo, start_pos, children_offsets);

}
