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


#include <iostream>
#include <fstream>
#include <string>
#include "stdio.h"

void preproc_file(const char* filename, std::ofstream& output);

inline void skip_whitespace(std::string& line, int& i) {
    while (i < line.length() && (line[i] == ' ' || line[i] == '\t')) i++;
}

#define MAX_FILENAME_LENGTH 512

inline void read_filename(char* filename_buf, int path_len, std::string& line, int& i) {
    int j = path_len;
    while (i < line.length() && line[i] != ' ' && line[i] != '\t' && line[i] != '\r') {
        filename_buf[j++] = line[i++];
        if (j >= MAX_FILENAME_LENGTH - 1) throw "File name too long.";
    }
    filename_buf[j] = '\0';
}

inline int file_location(char* location, const char* filepath) {
    int last_slash = 0;
    int len = 0;
    while (filepath[len] != '\0') {
        location[len] = filepath[len];
        if (location[len] == '/') last_slash = len + 1;
        len++;
        if (len >= MAX_FILENAME_LENGTH - 1) throw "File path too long.";
    }
    location[last_slash] = '\0';
    return last_slash;
}

bool try_include(const char* path, std::string& line, std::ofstream& output) {
    int i = 0;
    skip_whitespace(line, i);
    if (i + 9 < line.length() && line.substr(i, 9) == "//INCLUDE") {
        i += 9;
        skip_whitespace(line, i);

        char filename[MAX_FILENAME_LENGTH];
        int path_len = file_location(filename, path);
        read_filename(filename, path_len, line, i);
        preproc_file(filename, output);
        return true;
    }
    return false;
}

void preproc_file(const char* filename, std::ofstream& output) {
    std::string line;
    std::ifstream input(filename);

    if (input.is_open()) {
        while (getline(input, line)) {
            if (try_include(filename, line, output)) continue;
            output << line << std::endl;
        }
        input.close();
    } else {
        throw std::string("File \"") + std::string(filename) + std::string("\" not found.");
    }
}

void preproc(const char* in, const char* out) {
    std::remove(out);
    std::ofstream output(out);
    if (output.is_open()) {
        preproc_file(in, output);
        output.close();
    }
}

#ifdef CLI
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <file> <location>" << std::endl;
        return 1;
    }

    try {
        preproc(argv[1], argv[2]);
    } catch (std::string e) {
        std::cout << std::string("[SHADER PREPROC ERROR] ") + e << std::endl;
        return 1;
    }
    return 0;
}
#endif
