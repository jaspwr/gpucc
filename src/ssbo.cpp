#include "ssbo.h"

#include<stdio.h>
#include<stdlib.h>

Ssbo::Ssbo(GLuint size) // Zero-initialised
{
    this->size = size;
    glGenBuffers(1, &index);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, index);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

Ssbo::Ssbo(GLuint size, void* data) // Initialised with data
{
    this->size = size;
    glGenBuffers(1, &index);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, index);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

Ssbo::~Ssbo()
{
    glDeleteBuffers(1, &index);
}

void Ssbo::bind(GLuint index)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, this->index);
}

void* Ssbo::dump()
{
    void* data = malloc(size);
    glGetNamedBufferSubData(index, 0, size, data);
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