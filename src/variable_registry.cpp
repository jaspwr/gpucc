#include "variable_registry.h"

#include "utils.h"

#include <string.h>
#include <string>

std::string VariableRegistry::append_string_literal(char* data, u32 start, u32 length) {
    char* buffer = new char[length + 1];
    memcpy(buffer, data + start, length);
    buffer[length] = '\0';
    auto name = "0str" + std::to_string(string_literal_count++);
    
    global_scope.variables[name] = new TypedValue{
        name,
        const_string_type(length + 1),
        0,
        buffer,
        length + 1
    };
    return name;
}

TypedValue* check_scope(std::string& name, Scope& scope) {
    auto search = scope.variables.find(name);
    if (search != scope.variables.end()) {
        return search->second;
    } else {
        return nullptr;
    }
}

TypedValue* VariableRegistry::get_var(std::string& name) {
    auto scopes_iter = local_scopes.iter();
    while (scopes_iter.has_next()) {
        auto& scope = scopes_iter.next();
        auto search = check_scope(name, scope);
        if (search != nullptr) return search;
    }
    auto search = check_scope(name, global_scope);
    if (search != nullptr) return search;
    throw Exception("Unknown identifier \"" + name + "\".");
}

TypedValue::TypedValue(std::string name, CompilerType type, Register register_, void* data, u32 size) {
    this->name = name;
    this->type = type;
    this->register_ = register_;
    this->data = data;
    this->size = size;
}

TypedValue::TypedValue() {
    data = nullptr;
}

TypedValue::~TypedValue() {
    if (data != nullptr)
        delete[] (char*)data;
}

void VariableRegistry::add_var(std::string& name, TypedValue* value) {
    if (local_scopes.size() == 0) {
        global_scope.variables[name] = value;
        global_scope.loadable_registers.push_back(value->register_);
        return;
    }
    auto& current_scope = local_scopes.peek();
    current_scope.variables[name] = value;
    current_scope.loadable_registers.push_back(value->register_);
}

void VariableRegistry::add_enum_const(std::string& name, i64 value) {
    if (local_scopes.size() == 0) {
        global_scope.enum_consts[name] = value;
        return;
    }
    auto& current_scope = local_scopes.peek();
    current_scope.enum_consts[name] = value;
}

bool VariableRegistry::is_loadable(Register register_) {
    // TODO: make faster
    auto scopes_iter = local_scopes.iter();
    while (scopes_iter.has_next()) {
        auto& scope = scopes_iter.next();
        for (auto& reg : scope.loadable_registers) {
            if (reg == register_) return true;
        }
    }
    for (auto& reg : global_scope.loadable_registers) {
        if (reg == register_) return true;
    }
    return false;
}

bool VariableRegistry::is_in_global_scope() {
    return local_scopes.size() == 0;
}

Register VariableRegistry::get_new_register() {
    return ++new_register_next;
}

void VariableRegistry::push_scope() {
    local_scopes.push(Scope());
}

void VariableRegistry::pop_scope() {
    local_scopes.pop();
}

void TypedValue::dereference() {
    type.pointer_level--;
}

GLuint TypedValue::get_ir_type() {
    if (type.pointer_level > 0) {
        return IR_PTR;
    }
    switch (type.base_type) {
        case BaseType::i8:
            return IR_I8;
        case BaseType::i32:
            return IR_I32;
        case BaseType::f32:
            return IR_F32;
        case BaseType::void_:
            return IR_VOID;
        default:
            throw Exception("Unknown type.");
    }
}

i64 VariableRegistry::get_enum_const(std::string& name) {
    auto scopes_iter = local_scopes.iter();
    while (scopes_iter.has_next()) {
        auto& scope = scopes_iter.next();
        auto search = scope.enum_consts.find(name);
        if (search != scope.enum_consts.end()) {
            return search->second;
        }
    }
    auto search = global_scope.enum_consts.find(name);
    if (search != global_scope.enum_consts.end()) {
        return search->second;
    }
    throw Exception("Unknown enum constant \"" + name + "\".");
}

GLuint TypedValue::IR_I8 = 0;
GLuint TypedValue::IR_I32 = 0;
GLuint TypedValue::IR_F32 = 0;
GLuint TypedValue::IR_VOID = 0;
GLuint TypedValue::IR_PTR = 0;
