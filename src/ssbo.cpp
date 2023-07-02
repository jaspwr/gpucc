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


#include "ssbo.h"

#include<stdio.h>
#include<stdlib.h>

Ssbo::Ssbo(GLuint size) { // Zero-initialised
    this->size = size;
    glGenBuffers(1, &index);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, index);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glFinish();
}

Ssbo::Ssbo(GLuint size, void* data) { // Initialised with data
    this->size = size;
    glGenBuffers(1, &index);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, index);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glFinish();
}

Ssbo::~Ssbo() {
    glDeleteBuffers(1, &index);
    glFinish();
}

void Ssbo::bind(GLuint index) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, this->index);
    glFinish();
}

void* Ssbo::dump() {
    void* data = malloc(size);
    glGetNamedBufferSubData(index, 0, size, data);
    glFinish();
    return data;
}

void Ssbo::print_contents() {
    auto token_dump = (GLuint*)dump();
    auto len = size / sizeof(GLuint);
    for (int i = 0; i < len; i++) {
        printf("%d ", token_dump[i]);
    }
    printf("\n");
    free(token_dump);
}

void Ssbo::print_size(const char* name) {
    GLint64 size;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, index);
    glGetBufferParameteri64v(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &size);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glFinish();
    (void)printf("[%s] SSBO size: %ld KB\n", name, size / 1000);
}
