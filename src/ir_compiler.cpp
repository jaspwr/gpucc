#include "ir_compiler.h"

#include "ir_ssbo_format.h"

std::string token_from_id(GLuint id, IrTokenList& tokens) {
    for (IrToken token : tokens) {
        if (token.id == id)
            return token.name;
    }
    return "ERROR";
}

bool is_alphanum(char c) {
    return c >= 'a' && c <= 'z' 
        || c >= 'A' && c <= 'Z' 
        || c >= '0' && c <= '9' 
        || c == '_'
        ;
}


std::string extract_token_at(std::string& str, u32& pos) {
    std::string ret = "";
    if (pos >= str.length() || pos < 0 ) throw "Invalid source position reference from shaders.";
    while (pos < str.length() && is_alphanum(str[pos])) {
        ret += str[pos];
        pos++;
    }
    return ret;
}

std::string serialize_uir_to_readable(GLuint* ir, u32 ir_len, 
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
            if(++i >= ir_len) throw "Malformed reference from shaders.";
            token = std::string("%") + std::to_string(ir[i]);
            break;
        case IR_SOURCE_POS_REF:
            if(++i >= ir_len) throw "Malformed source pos ref from shaders.";
            token = extract_token_at(source, ir[i]);
            break;
        case IR_INSERSION:
            throw "Insersion tokens should not not end up in shader output.";
        case IR_SELF_REFERENCE:
            throw "Self reference tokens should not not end up in shader output.";
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