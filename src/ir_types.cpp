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


#include "ir_types.h"

#include "utils.h"

namespace ir_types {
    struct TypeName {
        std::string name;
        u32 id;
    };

    std::vector<TypeName> types = {
        {"u0", 0},
        {"u1", 1},
        {"u8", 2},
        {"u16", 3},
        {"u32", 4},
        {"u64", 5},
        {"i8", 6},
        {"i16", 7},
        {"i32", 8},
        {"i64", 9},
        {"f32", 10},
        {"f64", 11},
        {"x86_fp80", 12},
        {"_", 100},
        {"error_type", 4000},
    };

    std::string shader_defs() {
        auto ret = std::string();

        for (TypeName t : types) {
            if (!char_utils::string_is_var_name(t.name)
                || char_utils::is_numeric(t.name[0])) {

                continue;
            }

            ret += "#define TYPE_"
                + std::string(t.name)
                + " "
                + std::to_string(t.id)
                + "\n";
        }

        return ret;
    }

    std::string id_to_string(GLuint id) {
        for (TypeName t : types) {
            if (t.id == id) {
                return t.name;
            }
        }
        return "unknown_type";
    }

    GLuint string_to_id(std::string str) {
        for (TypeName t : types) {
            if (t.name == str) {
                return t.id;
            }
        }
        return 4000;
    }
}
