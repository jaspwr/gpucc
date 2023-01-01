#pragma once

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef int i32;
typedef long i64;
typedef float f32;
typedef double f64;

#include <string>
#include "gl.h"
#include "parse_tree.h"

#define MAX_PATH 260

struct StrSplit {
    char** spl;
    u32 len;
};

const char* load_file(const char* path);
std::string get_bin_dir();
UintString to_uint_string(std::string str);
UintString to_uint_string(std::vector<GLuint> str);
std::string uintstring_to_string(UintString str);
StrSplit str_split(const char* str, char separator);
void free_str_split(StrSplit str_split);