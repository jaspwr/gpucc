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
};