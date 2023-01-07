#include "gl_atomic_counter.h"

GLAtomicCounter::GLAtomicCounter(GLuint inital_value) {
    atomic_buffer = inital_value;
    glGenBuffers(1, &atomic_buffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_buffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

GLAtomicCounter::~GLAtomicCounter() {
    glDeleteBuffers(1, &atomic_buffer);
}

void GLAtomicCounter::bind(GLuint index) {
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, atomic_buffer);
}

void GLAtomicCounter::unbind(GLuint index) {
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, 0);
}

void GLAtomicCounter::reset() {
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_buffer);
    GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
                                            GL_MAP_WRITE_BIT | 
                                            GL_MAP_INVALIDATE_BUFFER_BIT | 
                                            GL_MAP_UNSYNCHRONIZED_BIT);
    ptr[0] = 0;
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}