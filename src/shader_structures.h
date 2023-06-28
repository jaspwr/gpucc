#pragma once

#include "include/glad/glad.h"

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
    GLuint load_depth;
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