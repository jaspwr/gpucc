#pragma once

#include "include/glad/glad.h"

class Ssbo {
    public:
        unsigned int size;
        GLuint index;
        Ssbo(GLuint size); // Zero-initialised
        Ssbo(GLuint size, void* data); // Initialised with data
        ~Ssbo();
        void bind(GLuint index);
        void* dump();
        void print_contents();
        void print_size(const char* name);
};