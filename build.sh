#!/bin/bash
# GCC build script for Linux
# Copy shaders from 'shaders' into raw c++ strings in 'stringified_shaders' 
cd shaders
for FILE in *; do
    echo 'R"(' | cat - $FILE  > temp && mv temp ../stringified_shaders/$FILE;
    echo ')"' >> ../stringified_shaders/$FILE;
done
cd ..
# Compile glad and main
#g++ -c libraries/include/glad/glad.c
g++ -o bin/meow glad.o -lglfw -ldl lang.cpp main.cpp gl.cpp shader.cpp ui.cpp yacc_parser.cpp token_tree_gen.cpp
# Run program (comment out if you don't want to run after compiling)
./bin/meow tests/test.meow