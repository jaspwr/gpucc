// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of Meow.
//
// Meow is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// Meow is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Meow. If not, see <https://www.gnu.org/licenses/>.


#include "compiler_type.h"
#include "exception.h"

CompilerType const_string_type(u32 length) {
    return CompilerType(BaseType::u8, 0, true, length, false, false, 0);
}

CompilerType::CompilerType( BaseType base_type, 
                            u32 pointer_level, 
                            bool is_array, 
                            u32 array_length, 
                            bool volatile_, 
                            bool register_, 
                            u32 struct_union_id) {
    this->base_type = base_type;
    this->pointer_level = pointer_level;
    this->is_array = is_array;
    this->array_length = array_length;
    this->volatile_ = volatile_;
    this->register_ = register_;
    this->struct_union_id = struct_union_id;
}


u32 base_type_size (BaseType type, u32 struct_union_id) {
    switch (type) {
        case BaseType::i8:
            return 1;
        case BaseType::i32:
            return 4;
        case BaseType::i64:
            return 8;
        case BaseType::u8:
            return 1;
        case BaseType::u32:
            return 4;
        case BaseType::u64:
            return 8;
        case BaseType::f32:
            return 4;
        case BaseType::f64:
            return 8;
        case BaseType::void_:
            return 0;
        case BaseType::structure:
            return 0; // TODO
        case BaseType::union_:
            return 0; // TODO
    }
}

u32 CompilerType::size() {
    if (pointer_level > 0) {
        return 8;
    }
    if (is_array) {
        return array_length * base_type_size(base_type, struct_union_id);
    }
    return base_type_size(base_type, struct_union_id);
}
