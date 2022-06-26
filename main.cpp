#include "inc.h"
#include "lang.cpp"
//#include "yacc_parser.cpp"

const char* tokenizer_source =
#include "stringified_shaders/tokenizer.glsl"
;

const char* ast_builder_source =
#include "stringified_shaders/ast_builder.glsl"
;

const char* arranger_source =
#include "stringified_shaders/arranger.glsl"
;


int main(int argc, char *argv[])
{
    printf(" /\\_/\\\n( o.o )   ~~ meowcc verion α 0.1 ~~\n > C < \n");


    gl_init();

    if(argc != 2)
    {
        if(argc == 1)
            print(PRINT_ERROR,"File not specified");
        else
            print(PRINT_ERROR,"Too many arguments");
        return 1;
    }

    if(compiler::regens_lang)
        lang::regen_lang();
    else
        lang::load_lang(compiler::preset);


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



    const int bind_size = 7;
    shader_binding shader_bindings[bind_size];
    // shader_bindings[0] = {0,c,GL_READ_ONLY,GL_RG32F};
    // shader_bindings[1] = {1,screenTex,GL_WRITE_ONLY,GL_RG32F};
    // shader_bindings[2] = {2,tree,GL_READ_ONLY,GL_RG32F};
    // shader_bindings[3] = {3,cst,GL_READ_ONLY,GL_RG32F};
    shader_bindings[0].texture = c;
    shader_bindings[0].access = GL_READ_ONLY;
    shader_bindings[0].format = GL_RGBA32F;
    shader_bindings[0].type = shader_binding_type::image;
    shader_bindings[2].texture = lang::vram_token_tree;
    shader_bindings[2].access = GL_READ_ONLY;
    shader_bindings[2].format = GL_RGBA32F;
    shader_bindings[2].type = shader_binding_type::image;
    shader_bindings[3].texture = lang::vram_cst;
    shader_bindings[3].access = GL_READ_ONLY;
    shader_bindings[3].format = GL_RGBA32F;
    shader_bindings[3].type = shader_binding_type::image;
    shader_bindings[5].texture = screenTex;
    shader_bindings[5].access = GL_WRITE_ONLY;
    shader_bindings[5].format = GL_RGBA32F;
    shader_bindings[5].type = shader_binding_type::image;

    shader_bindings[1].type = shader_binding_type::none;
    shader_bindings[4].type = shader_binding_type::none;


    GLuint pong_screen = create_ssbo(SCREEN_WIDTH * SCREEN_WIDTH * 3 * sizeof(int));
    shader_bindings[1].texture = pong_screen;
    shader_bindings[1].type = shader_binding_type::ssbo;


    shader shad(shader_bindings,6,&tokenizer_source);
    shad.exec(std::ceil(SCREEN_WIDTH / 8), std::ceil(SCREEN_HEIGHT / 4), 1, true);

    // GLuint pong_screen;
    // glCreateTextures(GL_TEXTURE_2D, 1, &pong_screen);
	// glTextureParameteri(pong_screen, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTextureParameteri(pong_screen, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTextureParameteri(pong_screen, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTextureParameteri(pong_screen, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTextureStorage2D(pong_screen, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);


    int _data[6000];
    for(int i = 0; i < 6000; i++)
        _data[i] = 0;
    _data[0] = 20;
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(_data), _data, GL_DYNAMIC_COPY); //sizeof(data) only works for statically sized C/C++ arrays.
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    int hi = 0;
    GLuint atomics;
    glGenBuffers(1, &atomics);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomics);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &hi, GL_DYNAMIC_DRAW); 
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 6, atomics);

    GLuint __screenTex = create_ssbo(SCREEN_WIDTH * SCREEN_WIDTH * 3 * sizeof(int));


    shader_bindings[0].texture = __screenTex;
    shader_bindings[0].type = shader_binding_type::ssbo;


    shader_bindings[4].texture = ssbo;
    shader_bindings[4].type = shader_binding_type::ssbo;



    shader ast_builder(shader_bindings,6,&ast_builder_source);
    bool ping_pong = true;
    for(int i = 0; i < 8; i++){
        if(ping_pong){
            shader_bindings[0].texture = __screenTex;
            shader_bindings[1].texture = pong_screen;

        }else{
            shader_bindings[0].texture = pong_screen;
            shader_bindings[1].texture = __screenTex;
        }
        ping_pong = !ping_pong;
        ast_builder.exec(std::ceil(SCREEN_WIDTH / 8), std::ceil(SCREEN_HEIGHT / 4), 1, true);
    }

    shader arranger(shader_bindings,6,&arranger_source);
    arranger.exec(std::ceil(SCREEN_WIDTH / 8), std::ceil(SCREEN_HEIGHT / 4), 1, false);


    glUseProgram(screenShaderProgram);
    glBindTextureUnit(0, screenTex);
    glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

    unsigned char outp[4];
    char pixel[(SCREEN_HEIGHT*SCREEN_WIDTH)/8];
    glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/8, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
    //get_from_vram(screenTex,4,&outp);

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    print(PRINT_SUCCESS,"Program compiled successfully");
    std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
    
    for(int i = 0; i < 500; i++){
        printf("%c",pixel[i]);
        
    }
    printf("\n");


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

