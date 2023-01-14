#include "ir_ssbo_format.h"
#include "exception.h"

#include <iostream>

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

void append_codegen_ssbo_entry(GLuint* codegen_ssbo, GLuint& codegen_ssbo_len,
    GLuint node_token, std::vector<std::string>& ir, ParseTree& ir_pt, IrTokenList* ir_tokens) {

    // Codegen SSBO structure
    // 0 -> 256 : Pointers to IR section that is inserted for nodes with the token at that index
    // IR section:
    //     0 -> 4 : Child offsets
    //     4 : Length of IR
    //     5 -> 5 + length : IR tokens

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
    append_children_offsets(codegen_ssbo, start_pos, children_offsets);

}