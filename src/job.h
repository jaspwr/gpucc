#pragma once

#include <string>
#include <vector>

enum Emit {
    none,
    mew_ir,
    llvm_ir,
    llvm_bc,
};

struct Job {
    std::vector<std::string> source_files;
    std::string output_file;
    std::string yacc;
    Emit emit;
    bool dbg;

    Job() {
        this->source_files = std::vector<std::string>();
        this->emit = Emit::none;
        this->dbg = false;
    }

    Job(std::string source_path) {
        this->source_files = std::vector<std::string>();
        this->source_files.push_back(source_path);
        this->emit = Emit::none;
        this->dbg = false;
    }
};