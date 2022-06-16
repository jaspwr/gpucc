#include "inc.h"
#include "lang.cpp"
//#include "yacc_parser.cpp"

const char* shader_source =
#include "stringified_shaders/pass1.glsl"
;


int main(int argc, char *argv[])
{
    printf(" /\\_/\\\n( o.o )   ~~ meowcc verion b 0.1 ~~\n > ^ < \n");


    gl_init();

    if(argc != 2)
    {
        if(argc == 1)
            print(PRINT_ERROR,"File not specified");
        else
            print(PRINT_ERROR,"Too many arguments");
        return 1;
    }


    auto start = std::chrono::high_resolution_clock::now();

	FILE *in=fopen(argv[1],"rb");
    if(in==NULL)
    {
        char prnt[] = "Could not read \"";
        strcat(prnt,argv[1]);
        strcat(prnt,"\"");
        print(PRINT_ERROR,prnt);
        return 1;
    }
    #define BUFFER_SIZE (1 * 1024 * 1024)
    #define ITERATIONS (10 * 1024)
    unsigned char buffer[BUFFER_SIZE]; // 1 MiB buffer
    int i, x;
    for(i = 0; i < ITERATIONS; ++i)
    {
        fread(buffer, BUFFER_SIZE, 1, in);
    }
    GLuint c = load_to_vram(buffer,30000,1,GL_RGBA32F, GL_RGBA);
    #undef BUFFER_SIZE 
    #undef ITERATIONS

	fclose(in);


    //taken from here

//     //TODO: make everything here preprocessed
//     flush_tree(true);
//     char * dictionary = R"(auto
// break
// case
// char
// const
// continue
// default
// do
// double
// else
// enum
// extern
// float
// for
// goto
// if
// inline 1, a
// int
// long
// register
// restrict 1, a
// return
// short
// signed
// sizeof
// static
// struct
// switch
// typedef
// union
// unsigned
// void
// volatile
// while
// _Alignas
// _Alignof
// _Atomic
// _Bool
// _Complex
// _Generic
// _Imaginary 
// _Noreturn 
// _Static_assert 
// _Thread_local)";

//     add_token(";");
//     add_token("intj");
//     add_token("float");
//     add_token("unsignededededddddddddddddddddddddddddddddddddd");
//     add_token(" ");
//     add_token("void");
//     add_token("int");
 

//     token_tree tt = token_tree_gen();
    
    // cst _cst = yacc_token_tree_gen(&tt);
    // GLuint tree = load_to_vram((unsigned char*)tt.data,258,tt.height,GL_RGBA32F, GL_RGBA);
    // GLuint cst = load_to_vram((unsigned char*)_cst.rows,CST_ROW_WIDTH,512,GL_RGBA32F, GL_RGBA);
    // free(tt.data);
    lang::regen_lang();

    shader shad;
    shader_binding shader_bindings[4];
    // shader_bindings[0] = {0,c,GL_READ_ONLY,GL_RG32F};
    // shader_bindings[1] = {1,screenTex,GL_WRITE_ONLY,GL_RG32F};
    // shader_bindings[2] = {2,tree,GL_READ_ONLY,GL_RG32F};
    // shader_bindings[3] = {3,cst,GL_READ_ONLY,GL_RG32F};
    shader_bindings[0].binding = 0;
    shader_bindings[0].texture = c;
    shader_bindings[0].access = GL_READ_ONLY;
    shader_bindings[0].format = GL_RGBA32F;
    shader_bindings[1].binding = 1;
    shader_bindings[1].texture = screenTex;
    shader_bindings[1].access = GL_WRITE_ONLY;
    shader_bindings[1].format = GL_RGBA32F;
    shader_bindings[2].binding = 2;
    shader_bindings[2].texture = lang::vram_token_tree;
    shader_bindings[2].access = GL_READ_ONLY;
    shader_bindings[2].format = GL_RGBA32F;
    shader_bindings[3].binding = 3;
    shader_bindings[3].texture = lang::vram_cst;
    shader_bindings[3].access = GL_READ_ONLY;
    shader_bindings[3].format = GL_RGBA32F;
    shad.init(shader_bindings,&shader_source);
    shad.exec(std::ceil(SCREEN_WIDTH / 8), std::ceil(SCREEN_HEIGHT / 4), 1);


    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    print(PRINT_SUCCESS,"Program compiled successfully");
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";


    glUseProgram(screenShaderProgram);
    glBindTextureUnit(0, screenTex);
    glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

    unsigned char outp[4];
    char pixel[(SCREEN_HEIGHT*SCREEN_WIDTH)/8];
    glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/8, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
    //get_from_vram(screenTex,4,&outp);
    for(int i = 0; i < (SCREEN_HEIGHT*SCREEN_WIDTH)/8 *0.02; i++){
        printf("%c",pixel[i]);
    }

    #ifdef DEBUG_WINDOW
		glfwSwapBuffers(window);
		glfwPollEvents();
        while (!glfwWindowShouldClose(window))
        {
        }
        glfwDestroyWindow(window);
    #endif

	glfwTerminate();
	return 0;
}

