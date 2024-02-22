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
