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