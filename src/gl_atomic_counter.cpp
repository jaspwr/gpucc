// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of gpucc.
//
// gpucc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// gpucc is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gpucc. If not, see <https://www.gnu.org/licenses/>.


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
