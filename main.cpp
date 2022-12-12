#include "inc.h"
#include "lang.h"

int main(int argc, char *argv[])
{
    
    char* file = parse_args(argc, argv);

    gl_init();

    if(compiler::regens_lang)
        lang::regen_lang();
    else
        lang::load_lang(compiler::preset);

    auto start = std::chrono::high_resolution_clock::now();
    GLuint c = compiler::preprocessor(file);
    int* raw_compilation = compiler::compile(c);
    ir_codegen::post_process(raw_compilation);
    delete[] raw_compilation;

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    print(PRINT_SUCCESS, "Program compiled successfully");
    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    free(lang::_parse_tree_extra);

	glfwTerminate();
	return 0;
}

