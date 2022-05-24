#include <stdio.h>
#include <stdlib.h>
#include <chrono> 

#include "shader.cpp"
#include "token_tree_gen.cpp"

const char* shader_source =
#include "stringified_shaders/pass1.comp"
;


int main(int argc, char *argv[])
{
    


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

    token_tree tt = token_tree_gen();
    // for(int i = 0; i < 256*4 + 50; i++){
    //     printf("%i\n",tt.data[i]);
    // }
    // printf("%i",tt.height);



    GLuint tree = load_to_vram(tt.data,258,tt.height,GL_RGBA32F, GL_RGBA);

    shader shad;
    shader_binding shader_bindings[3];
    shader_bindings[0].binding = 0;
    shader_bindings[0].texture = c;
    shader_bindings[0].access = GL_READ_ONLY;
    shader_bindings[0].format = GL_RGBA32F;
    shader_bindings[1].binding = 1;
    shader_bindings[1].texture = screenTex;
    shader_bindings[1].access = GL_WRITE_ONLY;
    shader_bindings[1].format = GL_RGBA32F;
    shader_bindings[2].binding = 2;
    shader_bindings[2].texture = tree;
    shader_bindings[2].access = GL_READ_ONLY;
    shader_bindings[2].format = GL_RGBA32F;
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

