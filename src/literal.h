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


#pragma once

#include "types.h"
#include "gl.h"
#include "exception.h"

#include <iostream>

enum class LiteralType: u8 {
    Nil,
    U8,
    U32,
    U64,
    I8,
    I32,
    I64,
    F32,
    F64,
};

class Literal {
    public:
        LiteralType type;
        union {
            u8 u8;
            u32 u32;
            u64 u64;
            i8 i8;
            i32 i32;
            i64 i64;
            f32 f32;
            f64 f64;
        } value;

        Literal(GLuint type, GLuint value_first_half, GLuint value_second_half) {
            if (type == -1) {
                throw Exception("Invalid literal type");
            }
            this->type = (LiteralType)type;
            printf("Literal type: %d\n", type);
            printf("Literal value: %x %x\n", value_first_half, value_second_half);
            switch (this->type) {
                case LiteralType::Nil:
                    break;
                case LiteralType::U8:
                    this->value.u8 = value_second_half;
                    break;
                case LiteralType::U32:
                    this->value.u32 = value_second_half;
                    break;
                case LiteralType::U64:
                    this->value.u64 = ((u64)value_second_half << 32) | (u64)value_second_half;
                    break;
                case LiteralType::I8:
                    this->value.i8 = value_second_half;
                    break;
                case LiteralType::I32:
                    this->value.i32 = value_second_half;
                    break;
                case LiteralType::I64:
                    this->value.i64 = ((u64)value_first_half << 32) | (u64)value_second_half;
                    break;
                case LiteralType::F32:
                    this->value.f32 = *(f32*)&value_second_half;
                    break;
                case LiteralType::F64:
                    auto tmp = ((u64)value_first_half << 32) | (u64)value_second_half;
                    this->value.f64 = *(f64*)&tmp;
                    break;
            }
        }

        void print() {
            switch (this->type) {
                case LiteralType::Nil:
                    std::cout << "nil\n";
                    break;
                case LiteralType::U8:
                    std::cout << "u8 " << (u32)this->value.u8 << "\n";
                    break;
                case LiteralType::U32:
                    std::cout << "u32 " << this->value.u32 << "\n";
                    break;
                case LiteralType::U64:
                    std::cout << "u64 " << this->value.u64 << "\n";
                    break;
                case LiteralType::I8:
                    std::cout << "i8 " << (i32)this->value.i8 << "\n";
                    break;
                case LiteralType::I32:
                    std::cout << "i32 " << this->value.i32 << "\n";
                    break;
                case LiteralType::I64:
                    std::cout << "i64 " << this->value.i64 << "\n";
                    break;
                case LiteralType::F32:
                    std::cout << "f32 " << this->value.f32 << "\n";
                    break;
                case LiteralType::F64:
                    std::cout << "f64 " << this->value.f64 << "\n";
                    break;
            }
        }
};
