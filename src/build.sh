#!/bin/bash
arglist="compiler.cpp cli.cpp shader_preproc.cpp exception.cpp instruction_selection_syntax.cpp string_template.cpp postprocessor.cpp preprocessor_macro.cpp job.cpp compiler_type.cpp variable_registry.cpp preprocessor.cpp debug_printing.cpp utils.cpp gl.cpp gl_atomic_counter.cpp shader.cpp parse_tree.cpp ir_ssbo_format.cpp ssbo.cpp ir_compiler.cpp lang.cpp yacc.cpp -o ../bin/meow glad.o `pkg-config --libs --cflags glfw3` -lm" &&
copy_shaders() {
    rm -rf '../bin/shaders' &&
    cp -r 'shaders' '../bin/shaders'
}

if [ "$1" = "debug" ]; then
    clang++ meow.cpp $arglist -g -O0 &&
    copy_shaders
else if [ "$1" = "run" ]; then
    clang++ meow.cpp $arglist &&
    copy_shaders &&
    prime-run ../bin/meow ../tests/random.c --dbg
else if [ "$1" = "justrun" ]; then
    prime-run ../bin/meow ../tests/random.c --dbg
else if [ "$1" = "test" ]; then
    clang++ -std=c++17 ../tests/tests.cpp $arglist -o ../bin/testbin &&
    copy_shaders &&
    prime-run ../bin/testbin &&
    rm ../bin/testbin
else
    clang++ meow.cpp $arglist -O3 &&
    copy_shaders
fi fi fi fi
