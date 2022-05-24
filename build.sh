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
g++ -c libraries/include/glad/glad.c
g++ main.cpp -o bin/meow glad.o -lglfw -ldl
# Run program (comment out if you don't want to run after compiling)
./bin/meow test.meow