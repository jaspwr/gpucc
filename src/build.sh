
arglist="meow.cpp compiler.cpp cli.cpp utils.cpp gl.cpp shader.cpp parse_tree.cpp ir_ssbo_format.cpp ssbo.cpp ir_compiler.cpp lang.cpp yacc.cpp -o ../bin/meow glad.o `pkg-config --libs --cflags glfw3` -lm" &&
copy_shaders() {
    rm -rf '../bin/shaders' &&
    cp -r 'shaders' '../bin'
}

if [ "$1" = "debug" ]; then
    clang++ $arglist -g -O0 &&
    copy_shaders
else if [ "$1" = "run" ]; then
    clang++ $arglist &&
    copy_shaders &&
    ../bin/meow ../tests/basic_expression/basic_expression.c
else
    clang++ $arglist -O3 &&
    copy_shaders
fi fi