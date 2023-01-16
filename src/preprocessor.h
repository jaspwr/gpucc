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