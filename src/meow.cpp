#include <stdio.h>

#include "cli.h"
#include "compiler.h"
#include "gl.h"
#include "parse_tree.h"

int main(int argc, char** argv)
{
    try {
        Gl::init();
        Job job = parse_args(argc, argv);
        Shaders shaders = Gl::compile_shaders();
        compile(job, shaders);
    } catch (const char* msg) {
        printf("\033[1;31mError\033[0m %s\n", msg);
        return 1;
    } catch (std::string msg) {
        printf("\033[1;31mError\033[0m %s\n", msg.c_str());
        return 1;
    }
    return 0;
}