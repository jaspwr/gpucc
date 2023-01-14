#include "variable_registry.h"

#include <string.h>
#include <string>

TypedValue::TypedValue(std::string name, Scope scope, CompilerType type, void* data, u32 size) {
    this->name = name;
    this->scope = scope;
    this->type = type;
    this->data = data;
    this->size = size;
}

std::string VariableRegistry::append_string_literal(char* data, u32 start, u32 length) {
    char* buffer = new char[length + 1];
    memcpy(buffer, data + start, length);
    buffer[length] = '\0';
    auto name = "str" + std::to_string(string_literal_count++);
    
    add_variable(
        TypedValue(
            name,
            Scope::Global,
            const_string_type(length + 1),
            buffer,
            length + 1
        )
    );
    return name;
}

void VariableRegistry::add_variable(TypedValue value) {
    //variables[value.name] = value;
}