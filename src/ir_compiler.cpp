#include "ir_compiler.h"

std::string token_from_id(GLuint id, IrTokenList& tokens) {
    for (IrToken token : tokens) {
        if (token.id == id)
            return token.name;
    }
    return "ERROR";
}

std::string serialize_uir_to_readable(GLuint* ir, u32 ir_len, IrTokenList& ir_tokens) {
    std::string ret = "";
    for (int i = 0; i < ir_len; i++) {
        if (ir[i] == 0) continue;
        ret += token_from_id(ir[i], ir_tokens);
        ret += " ";
    }
    return ret;
}