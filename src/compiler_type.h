#pragma once

#include "types.h"

enum class BaseType: u32 {
    i8,
    i32,
    i64,
    u8,
    u32,
    u64,
    f32,
    f64,
    void_,
    structure,
    union_,
};

struct CompilerType {
    BaseType base_type;
    u32 pointer_level;

    bool is_array;
    u32 array_length;

    bool volatile_;
    bool register_;

    u32 struct_union_id;

    u32 size();

    CompilerType() = default;
    CompilerType(BaseType base_type, u32 pointer_level, bool is_array, u32 array_length, bool volatile_, bool register_, u32 struct_union_id);
};

CompilerType const_string_type(u32 length);