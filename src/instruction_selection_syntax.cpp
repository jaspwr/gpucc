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


#include "instruction_selection_syntax.h"

#include "utils.h"
#include "exception.h"
#include "ir_types.h"
#include "ir_ssbo_format.h"
#include "shader_structures.h"

#include "arch/x86_64/opcodes.h"

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#define REPLACEMENTS_BUF_LEN 1000

enum class SchemaCtx: u32 {
    Match,
    Replacement
};

union IrType_u {
    IrType type;
    GLuint raw[3];
};

inline u32 get_vreg_id(u32& counter, std::unordered_map<std::string, u32>& map,
              std::string& name) {

    if (map.find(name) == map.end()) {
        map.insert({name, counter++});
    }
    return map[name];
}

struct TypeCheckingCase {
    u32 replacements_loc;
    std::vector<IrType_u> types;
};

typedef std::vector<TypeCheckingCase> TypeCheckingEntry;

Ssbo* create_type_checking_ssbo(std::vector<TypeCheckingEntry> entries) {
    #define TYPE_CHECKING_BUF_LEN 1000

    GLuint buf[TYPE_CHECKING_BUF_LEN];
    u32 len = 1;

    len += entries.size();

    u32 contents_table_index = 1;

    for (TypeCheckingEntry entry : entries) {
        #define check_overflow() if (len >= TYPE_CHECKING_BUF_LEN) { \
            throw Exception("Instruction selection schema too long. Increase `TYPE_CHECKING_BUF_LEN` in instruction_selection_syntax.cpp."); \
        }

        buf[contents_table_index] = len;
        printf("contents_table_index: %d := %d\n", contents_table_index, len);

        check_overflow();
        buf[len++] = entry.size();
        printf("buf[%d] := %d\n", len - 1, buf[len - 1]);

        GLuint* fail_jmp_loc = nullptr;

        for (TypeCheckingCase c : entry) {
            if (fail_jmp_loc != nullptr) {
                *fail_jmp_loc = len;
            }

            check_overflow();
            buf[len++] = c.types.size();
            check_overflow();
            fail_jmp_loc = &buf[len];
            buf[len++] = 0; // fail_jmp_loc
            check_overflow();
            buf[len++] = c.replacements_loc;

            printf("\tlen: %d, repl %d \n", (i32)c.types.size(), c.replacements_loc);

            for (IrType_u t : c.types) {
                for (GLuint i = 0; i < 3; i++) {
                    check_overflow();
                    buf[len++] = t.raw[i];
                }
                printf("\t\tbase: %d, ptr: %d, load: %d\n", t.type.base, t.type.pointer_depth, t.type.load_depth);
            }
        }

        contents_table_index++;

        #undef check_overflow
    }

    return new Ssbo(len * sizeof(GLuint), buf);
}

inline void append_match(std::vector<std::string>& match_unparsed,
    std::vector<std::string>& replacement_unparsed,
    ParseTree& ir_tokens, ParseTree& pt, GLint* replacements_buf,
    u32& replacements_buf_len, std::vector<TypeCheckingEntry>& type_checking) {

    // MATCH PARSE TREE
    // -------------------------------
    // [IR tokens] -> type_checking_loc


    // TYPE CHECKING SSBO
    // -------------------------------
    // 0 -> num_of_entries : pointers to entries
    // num_of_entries + 1 ... : entries

    // TYPE CHECKING SSBO ENTRY
    // -------------------------------
    // loc + 0 : cases count
    // loc + 1 ... : cases

    // TYPE CHECKING CASE
    // -------------------------------
    // loc + 0 : types_count
    // loc + 1 : fail_jmp_loc
    // loc + 2 : replacements_loc
    // loc + 3 ... : types


    // REPLACEMENTS SSBO ENTRY
    // -------------------------------
    // loc + 0 : len
    // loc + 1 ... : asm tokens

    u32 vreg_id_counter = 1;
    auto vreg_id_map = std::unordered_map<std::string, u32>();

    auto types = std::vector<IrType_u>();

    auto match = std::vector<GLuint>();

    for (std::string token : match_unparsed) {
        if (token[0] == '%') {
            StrSplit spl = str_split(token.c_str(), ':');

            if (spl.len != 2) {
                throw Exception("Virtual registers missing a type annotation.");
            }

            auto name = std::string(spl.spl[0]);
            u32 id = get_vreg_id(vreg_id_counter, vreg_id_map, name);
            u32 base_type = ir_types::string_to_id(std::string(spl.spl[1]));

            if (base_type == 4000) {
                throw Exception(std::string("Unknown type annotation \"") + spl.spl[1] + "\".");
            }

            match.push_back(IR_REFERNCE);
            match.push_back(id);

            u32 types_index = id - 1;

            while (types_index >= types.size()) {
                types.push_back(IrType_u());
            }

            types[types_index].type.base = base_type;
            types[types_index].type.pointer_depth = 0; // TODO
            types[types_index].type.load_depth = 0;

        } else {
            auto id = get_token_id(ir_tokens, (char*)token.c_str());
            match.push_back(id);
        }
    }

    GLint* len_pos = replacements_buf + replacements_buf_len;
    u32 replacements_buf_len_i = ++replacements_buf_len;

    for (std::string token : replacement_unparsed) {
        if (replacements_buf_len >= REPLACEMENTS_BUF_LEN) {
            throw Exception("Instruction selection schema too long. Increase `REPLACEMENTS_BUF_LEN` in instruction_selection_syntax.cpp.");
        }

        if (token[0] == '%') {
            u32 pre_vreg_id_counter = vreg_id_counter;
            u32 id = get_vreg_id(vreg_id_counter, vreg_id_map, token);
            if (pre_vreg_id_counter != vreg_id_counter) {
                throw Exception("Unknown virtual register \"" + token + "\".");
            }

            replacements_buf[replacements_buf_len++] = -id;
        } else {
            GLint opcode = opcode_of(token);
            replacements_buf[replacements_buf_len++] = opcode;
        }
    }

    *len_pos = replacements_buf_len - replacements_buf_len_i;

    u32 replacement_loc = replacements_buf_len_i - 1;

    UintString match_uint = to_uint_string(match);

    u32 type_checking_loc = pt.exec(match_uint);
    if (type_checking_loc == 0) {
        type_checking.push_back(TypeCheckingEntry());
        type_checking_loc = type_checking.size();
    }

    auto type_checking_case = TypeCheckingCase {
        replacement_loc,
        types
    };

    type_checking[type_checking_loc - 1].push_back(type_checking_case);

    auto new_entry = ParseTreeEntry {
        match_uint,
        type_checking_loc,
    };

    pt.append_entry(new_entry);
}


inline void handle_token(std::vector<std::string>& match,
    std::vector<std::string>& tokens, ParseTree& ir_tokens,
    SchemaCtx& ctx, std::string& token, ParseTree& pt,
    GLint* replacements_buf, u32& replacements_buf_len, std::vector<TypeCheckingEntry>& type_checking) {

    if (ctx == SchemaCtx::Match && token == "->") {
        ctx = SchemaCtx::Replacement;
        match = tokens;
        tokens = std::vector<std::string>();
    } else if (ctx == SchemaCtx::Replacement && token == ";") {
        ctx = SchemaCtx::Match;
        append_match(match, tokens, ir_tokens, pt, replacements_buf, replacements_buf_len, type_checking);
        tokens = std::vector<std::string>();
    } else {
        tokens.push_back(token);
    }
}

InstSelRet parse_instruction_selection(const char* schema, ParseTree& ir_tokens) {
    try {
        auto pt = ParseTree(1000);

        auto token = std::string();
        auto tokens = std::vector<std::string>();

        auto match = std::vector<std::string>();

        auto type_checking = std::vector<TypeCheckingEntry>();

        GLint replacements_buf[REPLACEMENTS_BUF_LEN];
        GLuint replacements_buf_len = 0;

        auto ctx = SchemaCtx::Match;
        for (u32 i = 0; schema[i] != '\0'; i++) {
            char c = schema[i];
            if (char_utils::is_whitespace(c)) {
                if (token.length() > 0) {
                    handle_token(match, tokens, ir_tokens, ctx, token, pt, replacements_buf,
                                 replacements_buf_len, type_checking);
                    token = std::string();
                }
            } else {
                token += c;
            }
        }

        Ssbo* matches_ssbo_ptr = pt.into_ssbo();
        Ssbo* type_checking_ssbo_ptr = create_type_checking_ssbo(type_checking);
        Ssbo* replacements_ssbo_ptr = new Ssbo(replacements_buf_len * sizeof(GLint), replacements_buf);

        return InstSelRet {
            matches_ssbo_ptr,
            type_checking_ssbo_ptr,
            replacements_ssbo_ptr
        };
    } catch (Exception e) {
        e.type = ExceptionType::InstructionSelectionSchema;
        throw e;
    }
}
