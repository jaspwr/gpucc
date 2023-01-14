#include <stdio.h>

#include "cli.h"
#include "compiler.h"
#include "gl.h"
#include "parse_tree.h"
#include "exception.h"

int main(int argc, char** argv) {
    try {
        Gl::init(true);
        Job job = parse_args(argc, argv);
        Shaders shaders = Gl::compile_shaders();
        compile(job, shaders);
    } catch (Exception& e) {
        e.print();
        return 1;
    }
    return 0;
}