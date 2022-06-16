#include "shader.h"

//#define OUTPUT_DISASSEMBLY


void shader::init(shader_binding *_shader_bindings, const char **shader_source){
    index = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(index, 1, shader_source, NULL);
    glCompileShader(index);
    GLint isCompiled = 0;
    glGetShaderiv(index, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(index, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(index, maxLength, &maxLength, &errorLog[0]);
        for(int i = 0; i < maxLength; i ++)
            printf("%c",(char)errorLog[i]);
        
        glDeleteShader(index);
    }

    program_index = glCreateProgram();

    #ifdef OUTPUT_DISASSEMBLY
        const size_t MAX_SIZE = 1<<24;
        char binary[MAX_SIZE];
        GLenum format;
        GLint length;
        glGetProgramBinary(program_index,MAX_SIZE,&length,&format,&binary[0]);
        std::ofstream binaryfile("bin.txt");
        binaryfile.write(binary,length);
    #endif

    glAttachShader(program_index, index);
    glLinkProgram(program_index);

    shader_bindings = _shader_bindings;


}


void shader::exec(GLuint dsp_x, GLuint dsp_y, GLuint dsp_z){
    glUseProgram(program_index);
    int _i = 0;
    int size = sizeof(shader_bindings)/ (sizeof(shader_binding)/8);
    printf("%i\n",size);
    for(_i = 0; _i < size; _i++)
    {
        glBindImageTexture(shader_bindings[_i].binding, shader_bindings[_i].texture, 
                            0, GL_FALSE, 0, shader_bindings[_i].access, 
                            shader_bindings[_i].format);
    }
    glDispatchCompute(dsp_x, dsp_y, dsp_z);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

