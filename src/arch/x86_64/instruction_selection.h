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


const char* inst_selection_schema = R"(
%a:i32 = ADD %b:i32 %c:i32 ->
    mov %a %b
    add %a %c
    ;

%a:f32 = ADD %b:f32 %c:f32 ->
    mov %a %b
    addss %a %c
    ;

%a:f32 = MUL %b:f32 %c:f32 ->
    mov %a %b
    mulss %a %c
    ;

%a:i32 = MUL %b:i32 %c:i32 ->
    mov %a %b
    imul %a %c
    ;


JMP %a:_ ->
    jmp %a
    ;

%a:_ = COPY %b:_ ->
    mov %a %b
    ;

%a:u1 = CMP EQ %b:i32 %c:i32
JZ %a:u1 %d:_ ->
    cmp %b %c
    jne %d
    ;
)";
