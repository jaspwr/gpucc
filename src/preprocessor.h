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

#include "extendable_buffer.h"
#include "variable_registry.h"
#include "preprocessor_macro.h"


#include <string>
#include <unordered_map>



// Public interface
std::string preprocess(std::string filename, VariableRegistry& var_reg);

namespace preprocessor {
    typedef std::unordered_map<std::string, PreprocessorMacro*> PreProcTokensMap;

    void preprocess(const char* filename, ExtendableBuffer<u8>* buffer, std::vector<std::string>& included,
        VariableRegistry& var_reg, PreProcTokensMap& pre_proc_tokens);
}
