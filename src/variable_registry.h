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

#include "types.h"
#include "compiler_type.h"
#include "stack.h"
#include "gl.h"

#include <string>
#include <vector>
#include <unordered_map>


typedef u32 Register;

struct TypedValue {
    static GLuint IR_I8;
    static GLuint IR_I32;
    static GLuint IR_F32;
    static GLuint IR_VOID;
    static GLuint IR_PTR;


    std::string name;
    CompilerType type;

    Register register_;

    void* data;
    u32 size;

    TypedValue();
    TypedValue(std::string name, CompilerType type, Register register_, void* data, u32 size);
    ~TypedValue();

    void dereference();

    GLuint get_ir_type();
};


struct Scope {
    std::unordered_map<std::string, TypedValue*> variables;
    std::unordered_map<std::string, i64> enum_consts;
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
        i64 get_enum_const(std::string& name);
        void add_var(std::string& name, TypedValue* value);
        void add_enum_const(std::string& name, i64 value);
        bool is_loadable(Register register_);
        bool is_in_global_scope();
        Register get_new_register();

        Register new_register_next;
};
