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


#pragma once

#include "types.h"

#include <string>
#include "gl.h"
#include "parse_tree.h"

#define MAX_PATH 260

struct StrSplit {
    // TODO: Make these a class that clean themselves
    //       up in a destructor.
    char** spl;
    u32 len;
};

const char* load_file(const char* path);
void write_file(const char* path, const char* content);
std::string repeated_char(u32 length, char c);
std::string get_bin_dir();
UintString to_uint_string(std::string str);
UintString to_uint_string(std::vector<GLuint> str);
std::string uintstring_to_string(UintString str);
StrSplit str_split(const char* str, char separator);
void free_str_split(StrSplit str_split);
u32 parse_int(char* str);
std::string extract_token_at(std::string& str, u32 pos);

namespace char_utils {
    bool is_alpha(char c);
    bool is_numeric(char c);
    bool is_alpha_numeric_or_underscore(char c);
    bool is_whitespace(char c);
    bool string_is_var_name(std::string str);
    bool string_is_var_name(char* str);
}

// template <typename T>
// struct Option {
//     enum { None, Some } type;
//     T* value_ptr;

//     Option() {
//         value_ptr = nullptr;
//         type = None;
//     }

//     Option(T* value_ptr) {
//         this->value_ptr = value_ptr;
//     }

//     bool is_some() {
//         return type == Some;
//     }

//     bool is_none() {
//         return type == None;
//     }

//     T* unwrap() {
//         if (type == None) {
//             throw "Unwrapping None.";
//         }
//         return value_ptr;
//     }
// };
