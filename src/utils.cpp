#include "utils.h"

#include <iostream>
#include <fstream>
#include <string>

#define BAD_FILE_MSG "Unable to read file."

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
        throw BAD_FILE_MSG;
    
    if (file_buffer == nullptr)
        throw BAD_FILE_MSG;
    
    return file_buffer;
}

std::string get_bin_dir()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");

    return std::string(buffer).substr(0, pos);
}

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