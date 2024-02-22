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


#include "preprocessor_macro.h"
#include "exception.h"
#include "utils.h"

#include <stdio.h>

ObjectLikeMacro::ObjectLikeMacro(std::string value) {
    this->value = value;
}

void ObjectLikeMacro::execute(ExtendableBuffer<u8>* buffer, u32& i, const char* source) {
    buffer->add((u8*)value.c_str(), value.size());
}

std::string extract_args_str (u32& i, const char* source) {
    while (char_utils::is_whitespace(source[i])) {
        i++;
        if (source[i] == '\0') throw Exception("Expected '(' after macro name");
    }
    if (source[i++] != '(') throw Exception("Expected '(' after macro name");
    std::string args = "";
    while (source[i] != ')') {
        if (source[i] == '\0') throw Exception("Expected ')' after macro arguments");
        args += source[i];
        i++;
    }
    i++;
    return args;
}

std::string trim_whitespace(std::string str) {
    u32 start = 0;
    u32 end = str.size() - 1;
    while (char_utils::is_whitespace(str[start])) start++;
    while (char_utils::is_whitespace(str[end])) end--;
    return str.substr(start, end - start + 1);
}

std::vector<std::string> extract_args(u32& i, const char* source) {
    // Slow but whatever

    auto args_str = extract_args_str(i, source);

    auto args = std::vector<std::string>();
    std::size_t pos = args_str.find(',');
    while (pos != std::string::npos) {
        auto arg = args_str.substr(0, pos);
        args.push_back(trim_whitespace(arg));
        args_str = args_str.substr(pos + 1);
        pos = args_str.find(',');
    }
    args.push_back(trim_whitespace(args_str));
    return args;
}

#define NOT_ARG -1
i32 is_arg(std::string arg, std::vector<std::string> args) {
    for (u32 i = 0; i < args.size(); i++) {
        if (arg == args[i]) return i;
    }
    return NOT_ARG;
}

void push_alphanum_arg(std::string plain_string_buffer, std::vector<std::string> args, StringTemplate& template_value) {
    if (plain_string_buffer != "") {
        if (plain_string_buffer[0] == '#') {
            plain_string_buffer = plain_string_buffer.substr(1);
            throw Exception("TODO: implement macro argument stringizing");
        }
        auto arg_index = is_arg(plain_string_buffer, args);
        if (arg_index != NOT_ARG)
            template_value.push(new InsertionTemplateItem(arg_index));
        else
        // TODO: allow `#`
            template_value.push(new PlainStringTemplateItem(plain_string_buffer));
    }
}

FunctionLikeMacro::FunctionLikeMacro(std::string value) {
    u32 i = 0;
    auto args = extract_args(i, value.c_str());
    template_value = StringTemplate();

    std::string plain_string_buffer = "";
    for (;i < value.size(); i++) {
        if (char_utils::is_alpha_numeric_or_underscore(value[i]) || value[i] == '#') {
            plain_string_buffer += value[i];
        } else {
            push_alphanum_arg(plain_string_buffer, args, template_value);
            template_value.push(new PlainStringTemplateItem(value.substr(i, 1))); // I don't like this but eh
            plain_string_buffer = "";
        }
    }
    push_alphanum_arg(plain_string_buffer, args, template_value);

}

void FunctionLikeMacro::execute(ExtendableBuffer<u8>* buffer, u32& i, const char* source) {
    auto args = extract_args(i, source);

    auto value = template_value.render(args);
    buffer->add((u8*)value.c_str(), value.size());
}
