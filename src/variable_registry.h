#pragma once

#include "types.h"
#include "compiler_type.h"
#include "stack.h"

#include <string>
#include <vector>
#include <unordered_map>


typedef u32 Register;

struct TypedValue {
    std::string name;
    CompilerType type;

    Register register_;

    void* data;
    u32 size;

    TypedValue();
    TypedValue(std::string name, CompilerType type, Register register_, void* data, u32 size);
    ~TypedValue();
};

struct Scope {
    std::unordered_map<std::string, TypedValue*> variables;
    std::vector<Register> loadable_registers;

    Scope() {
        loadable_registers = std::vector<Register>();
    }

    ~Scope() {
        for (auto& pair: variables) {
            delete pair.second;
        }
    }
};

class VariableRegistry {
    private:
        u32 string_literal_count;
        Scope global_scope;
        Stack<Scope> local_scopes = Stack<Scope>("VariableRegistry::local_scopes");
    public:
        std::string append_string_literal(char* data, u32 start, u32 length);
        
        void push_scope();
        void pop_scope();
        
        TypedValue* get_var(std::string& name);
        void add_var(std::string& name, TypedValue* value);
        bool is_loadable(Register register_);
        Register get_new_register();

        Register new_register_next;
};