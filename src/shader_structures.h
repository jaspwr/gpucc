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

#include <glad/glad.h>

// TODO: encase these in a namespace

struct Token {
    GLuint id;
    GLuint len;
    GLint ast_node_location;

    void print() {
        printf("token: %d, len: %d, ast_node_location: %d\n", id, len, ast_node_location);
    }
};

struct ChildNode {
    GLint ref;
    GLuint codegenVolume;

    void print() {
        printf("ref: %d, codegenVolume: %d\n", ref, codegenVolume);
    }
};

struct IrType {
    GLuint base;
    GLuint pointer_depth;
    GLuint qualifiers;
};

struct LiveInterval {
    GLuint start;
    GLuint end;
};

struct AstNode {
    GLuint nodeToken;
    ChildNode children[4];
    GLuint volume;
    GLuint parent;

    void print(std::string name) {
        printf("nodeToken: %s, volume: %d, parent: %d\n", name.c_str(), volume, parent);
        for (auto child : children) {
            if (child.ref == 0) continue;
            printf("    ");
            child.print();
        }
    }
};
