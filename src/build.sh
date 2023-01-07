#!/bin/bash
arglist="compiler.cpp cli.cpp debug_printing.cpp utils.cpp gl.cpp gl_atomic_counter.cpp shader.cpp parse_tree.cpp ir_ssbo_format.cpp ssbo.cpp ir_compiler.cpp lang.cpp yacc.cpp -o ../bin/meow glad.o `pkg-config --libs --cflags glfw3` -lm" &&
copy_shaders() {
    rm -rf '../bin/shaders' &&
    cp -r 'shaders' '../bin'
}

if [ "$1" = "debug" ]; then
    clang++ meow.cpp $arglist -g -O0 &&
    copy_shaders
else if [ "$1" = "run" ]; then
    clang++ meow.cpp $arglist &&
    copy_shaders &&
    prime-run ../bin/meow ../tests/random.c --dbg
else if [ "$1" = "test" ]; then
    clang++ -std=c++17 ../tests/tests.cpp $arglist -o ../bin/testbin &&
    copy_shaders &&
    prime-run ../bin/testbin &&
    rm ../bin/testbin
else
    clang++ $arglist -O3 &&
    copy_shaders
fi fi fi
