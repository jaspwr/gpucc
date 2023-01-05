#pragma once

#include "types.h"

#include <string>
#include "gl.h"
#include "parse_tree.h"

#define MAX_PATH 260

struct StrSplit {
    char** spl;
    u32 len;
};

const char* load_file(const char* path);
std::string repeated_char(u32 length, char c);
std::string get_bin_dir();
UintString to_uint_string(std::string str);
UintString to_uint_string(std::vector<GLuint> str);
std::string uintstring_to_string(UintString str);
StrSplit str_split(const char* str, char separator);
void free_str_split(StrSplit str_split);