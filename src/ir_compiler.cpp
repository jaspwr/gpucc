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


#include "ir_compiler.h"
#include "ir_ssbo_format.h"
#include "exception.h"

std::string token_from_id(GLuint id, IrTokenList& tokens) {
    for (IrToken token : tokens) {
        if (token.id == id)
            return token.name;
    }
    return "ERROR";
}


std::string serialize_uir_to_readable(const GLuint* ir, u32 ir_len,
    IrTokenList& ir_tokens, std::string& source) {

    std::string ret = "";

    // Ignore trailing whitespace
    while (ir[ir_len - 1] == 0) {
        ir_len--;
    }

    for (int i = 0; i < ir_len; i++) {
        std::string token = "";
        switch (ir[i]) {
        case IR_REFERNCE:
            if(++i >= ir_len) throw Exception("Malformed reference from shaders.");
            token = std::string("%") + std::to_string(ir[i]);
            break;
        case IR_SOURCE_POS_REF: {
            if(++i >= ir_len) throw Exception("Malformed source pos ref from shaders.");
            u32 pos = ir[i];
            token = extract_token_at(source, pos);
            break; }
        case IR_INSERSION:
            throw Exception("Insersion tokens should not not end up in shader output.");
        case IR_SELF_REFERENCE:
            throw Exception("Self reference tokens should not not end up in shader output.");
        case IR_LITERAL_REF:
            if(++i >= ir_len) throw Exception("Malformed literal marker from shaders.");
            token = std::string("&") + std::to_string(ir[i]);
            break;
        default:
            token = ir[i] == 0
                ? "NULL"
                : token_from_id(ir[i], ir_tokens)
            ;
            break;
        }
        ret += token + (token != "\n" ? " " : "");
    }
    return ret;
}

std::string ir_tokens_shader_definitions(IrTokenList& ir_tokens) {
    std::string ret = "";
    for (IrToken token : ir_tokens) {
        if (!char_utils::string_is_var_name(token.name)) continue;
        ret += "#define IR_" + token.name + " " + std::to_string(token.id) + "\n";
    }
    return ret;
}
