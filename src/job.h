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

#include <string>
#include <vector>
#include "types.h"

enum class Emit: u32 {
    none,
    mew_ir,
    llvm_ir,
    llvm_bc,
};

struct Job {
    static std::string libc_path;

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
