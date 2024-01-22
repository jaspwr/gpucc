#!/bin/bash

if [ ! -d "src" ]; then
    echo "Please run this script from the root directory of the project."
    exit 1
fi

cd src

c_compiler="clang"
compiler="clang++"

if [ ! -d "../bin" ]; then
    mkdir "../bin"
fi

if [ ! -d "../bin/shaders" ]; then
    mkdir "../bin/shaders"
fi

if [ ! -f "glad.o" ]; then
    $c_compiler -c ../glad.c -o glad.o
fi

arglist="-I../include compiler.cpp arch/x86_64/opcodes.cpp cli.cpp shader_errors.cpp arch/x86_64/registers.cpp shader_preproc.cpp ir_types.cpp exception.cpp instruction_selection_syntax.cpp string_template.cpp preprocessor_macro.cpp job.cpp compiler_type.cpp variable_registry.cpp preprocessor.cpp debug_printing.cpp utils.cpp gl.cpp gl_atomic_counter.cpp shader.cpp parse_tree.cpp ir_ssbo_format.cpp ssbo.cpp ir_compiler.cpp lang.cpp yacc.cpp -o ../bin/meow glad.o `pkg-config --libs --cflags glfw3` -lm" &&
copy_shaders() {
    rm -rf '../bin/shaders' &&
    cp -r 'shaders' '../bin/shaders'
}

if [ "$1" = "debug" ]; then
    $copiler meow.cpp $arglist -g -O0 &&
    copy_shaders
else if [ "$1" = "run" ]; then
    $compiler meow.cpp $arglist &&
    copy_shaders &&
    prime-run ../bin/meow ../tests/random.c --dbg
else if [ "$1" = "justrun" ]; then
    prime-run ../bin/meow ../tests/random.c --dbg
else if [ "$1" = "test" ]; then
    $compiler -std=c++17 ../tests/tests.cpp $arglist -o ../bin/testbin &&
    copy_shaders &&
    prime-run ../bin/testbin &&
    rm ../bin/testbin
else
    $compiler meow.cpp $arglist -O3 &&
    copy_shaders
fi fi fi fi
