#pragma once

#include "include/glad/glad.h"

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

struct AstNode {
	GLuint nodeToken;
	ChildNode children[4];
    GLuint volume;

    void print() {
        printf("nodeToken: %d, volume: %d\n", nodeToken, volume);
        for (auto child : children) {
            if (child.ref == 0) continue;
            printf("    ");
            child.print();
        }
    }
};