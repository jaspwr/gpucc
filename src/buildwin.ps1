clang meow.cpp compiler.cpp cli.cpp utils.cpp gl.cpp shader.cpp parse_tree.cpp ir_ssbo_format.cpp ssbo.cpp ir_compiler.cpp lang.cpp yacc.cpp -o ../bin/meow.exe glad.o -lglfw3dll
if($LASTEXITCODE -eq 0) {
    rm -Recurse -Force '../bin/shaders'
    Copy-Item 'shaders' '../bin' -Recurse
    ../bin/meow.exe ../tests/basic_expression/basic_expression.c
}