#pragma once

#include "types.h"
#include "compiler_type.h"

#include <string>
#include <unordered_map>

enum class Scope: u32 {
    Global,
    Local,
};

struct TypedValue {
    std::string name;
    Scope scope;
    CompilerType type;

    void* data;
    u32 size;

    TypedValue(std::string name, Scope scope, CompilerType type, void* data, u32 size);
};

class VariableRegistry {
    private:
        u32 string_literal_count;
        //std::unordered_map<std::string, TypedValue> variables;
    public:
        std::string append_string_literal(char* data, u32 start, u32 length);
        void add_variable(TypedValue value);
};