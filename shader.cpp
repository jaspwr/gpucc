#include "shader.h"
#include <Windows.h>
#include <string>
#include "utils.h"


std::string get_bin_dir()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");

    return std::string(buffer).substr(0, pos);
}

std::string readFile(const char* filePath) {
    std::string content;

    auto path_full = get_bin_dir() + "/" + std::string(filePath);
    std::ifstream fileStream(path_full, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Could not read file " << path_full << ". File does not exist." << std::endl;
        throw;
        return "";
    }

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

shader::shader( shader_binding* _shader_bindings, 
                int _binds_count, 
                const char* shader_source_file) {
    index = glCreateShader(GL_COMPUTE_SHADER);
    std::string shader_source = readFile(shader_source_file);
    const char* c_shader_source = shader_source.c_str();
    glShaderSource(index, 1, &c_shader_source, NULL);
    glCompileShader(index);
    GLint isCompiled = 0;//#define OUTPUT_DISASSEMBLY
    glGetShaderiv(index, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE) {
        printf("Failed to compile \"%s\"\n", shader_source_file);
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
    binds_count = _binds_count;


}


void shader::exec( GLuint dsp_x,
                   GLuint dsp_y,
                   GLuint dsp_z, 
                   bool rebind) {
    glUseProgram(program_index);
    if(rebind){
        int _i = 0;
        //int size = sizeof(shader_bindings)/ (sizeof(shader_binding)/8);
        //printf("%i\n",size);
        for(_i = 0; _i < binds_count; _i++)
        {
            switch(shader_bindings[_i].type){
            case shader_binding_type::image:
                glBindImageTexture(_i, shader_bindings[_i].texture, 
                                    0, GL_FALSE, 0, shader_bindings[_i].access, 
                                    shader_bindings[_i].format);
                break;
            case shader_binding_type::ssbo:
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _i, shader_bindings[_i].texture);
                break;
            case shader_binding_type::none:
                break;
            }
        }
    }
    glDispatchCompute(dsp_x, dsp_y, dsp_z);
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

