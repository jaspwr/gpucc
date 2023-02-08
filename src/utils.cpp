#include "utils.h"
#include "exception.h"

#include <iostream>
#include <fstream>
#include <string>

#define BAD_FILE_MSG "Unable to read file \""

const char* load_file (const char* path) {
    u64 size;
    char* file_buffer = nullptr;

    std::ifstream file(path, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = (u64)file.tellg();
        file_buffer = new char[size + 1];
        file.seekg(0, std::ios::beg);
        file.read(file_buffer, size);
        file.close();
        file_buffer[size] = '\0';
    } else 
        throw Exception(ExceptionType::File, BAD_FILE_MSG + std::string(path) + "\".");
    
    if (file_buffer == nullptr)
        throw Exception(ExceptionType::File, BAD_FILE_MSG + std::string(path) + "\".");
    
    return file_buffer;
}

#ifdef WIN32
#include <Windows.h>
std::string get_bin_dir() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");

    return std::string(buffer).substr(0, pos);
}
#endif

#ifdef __linux__
#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>

std::string get_bin_dir() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *path;
    if (count != -1) {
        path = dirname(result);
    }
    return std::string(path);
}
#endif

UintString to_uint_string(std::string str) {
    u32 length = str.length();
    GLuint* data = new GLuint[length];
    for (int i = 0; i < length; i++)
    {
        data[i] = (GLuint)str[i];
    }
    return { data, (GLuint)length };
}

std::string uintstring_to_string(UintString str) {
    std::string ret = "";
    for (int i = 0; i < str.length; i++)
    {
        ret += (char)str.data[i];
    }
    return ret;
}

UintString to_uint_string(std::vector<GLuint> str) {
    u32 length = str.size();
    GLuint* data = new GLuint[length];
    for (int i = 0; i < length; i++)
    {
        data[i] = str[i];
    }
    return { data, (GLuint)length };
}

StrSplit str_split(const char* str, char separator) {
    u32 count = 1;
    for (u32 i = 0; str[i] != '\0'; i++) {
        if (str[i] == separator) {
            count++;
        }
    }
    char** return_buffer = new char*[count];
    u32 index = 0;
    u32 str_len = 0;
    for (u32 i = 0; i < 10000; i++) {
        bool is_null_char = str[i] == '\0';
        if (str[i] == separator || is_null_char) {
            return_buffer[index] = new char[str_len + 1];
            return_buffer[index][str_len] = '\0';
            u32 ini_str_len = str_len;
            while (str_len > 0) {
                return_buffer[index][ini_str_len - str_len] = str[i - str_len];
                str_len--;
            }
            index++;
        } else {
            str_len++;
        }
        if (is_null_char) {
            break;
        }
    }
    return { return_buffer, count };
}

void free_str_split(StrSplit str_split) {
    for (i32 i = 0; i < str_split.len; i++) {
        delete[] str_split.spl[i];
    }
    // TODO: delete the spl var
}

std::string repeated_char(u32 length, char c) {
    // Surely there is a decrelative way to do this...
    std::string ret = "";
    for (u32 i = 0; i < length; i++) {
        ret += c;
    }
    return ret;
}

u32 parse_int(char* str) {
    u32 ret = 0;
    while (*str >= '0' && *str <= '9') {
        ret *= 10;
        ret += *str - '0';
        str++;
    }
    return ret;
}

namespace char_utils {
    bool is_alpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool is_numeric(char c) {
        return (c >= '0' && c <= '9');
    }

    bool is_alpha_numeric_or_underscore(char c) {
        return is_alpha(c) || is_numeric(c) || c == '_';
    }

    bool is_whitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
}

std::string extract_token_at(std::string& str, u32 pos) {
    std::string ret = "";
    if (str[pos] == '\'') {
        pos++;
        while (pos < str.length() && str[pos] != '\'') {
            ret += str[pos];
            pos++;
        }
        return ret;
    }
    if (pos >= str.length() || pos < 0 ) throw Exception("Invalid source position reference from shaders.");
    while (pos < str.length() && char_utils::is_alpha_numeric_or_underscore(str[pos])) {
        ret += str[pos];
        pos++;
    }
    return ret;
}