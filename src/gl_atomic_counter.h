#pragma once

#include "gl.h"

class GLAtomicCounter {
    public:
        GLAtomicCounter(GLuint inital_value);
        ~GLAtomicCounter();

        void bind(GLuint index);
        void unbind(GLuint index);

        void reset();
    private:
        GLuint id;
        GLuint atomic_buffer;
}; 