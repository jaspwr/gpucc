#include "gl.h"

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

GLFWwindow* window;


GLuint load_to_vram(unsigned char data[],int wid, int hei, GLint internal_format, GLenum format){
    GLuint img;
    glCreateTextures(GL_TEXTURE_2D, 1, &img);
    glBindTexture(GL_TEXTURE_2D, img);
    //TODO: change to 1D or make use of height
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format,wid, hei, 0, 
                 format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTextureParameteri(img, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(img, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(img, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(img, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return img;
}


GLuint create_ssbo(int size){
	int* _data = (int*)malloc(size*sizeof(int));
    for(int i = 0; i < size; i++)
        _data[i] = 0;
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(int), _data, GL_DYNAMIC_COPY);
	free(_data);
	return ssbo;
}

GLuint load_to_ssbo(int size, int type_size, int* data) {
	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size * type_size, data, GL_DYNAMIC_COPY);
	return ssbo;
}

GLuint create_vram_image(int data_length,  GLenum format){
    GLuint img;
    glCreateTextures(GL_TEXTURE_2D, 1, &img);
    glTextureParameteri(img, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(img, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(img, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(img, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //TODO: change to 1D or make use of height
	glTextureStorage2D(img, 1, format, data_length, 1);
    return img;
}

bool vSync = true;


void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
	if(severity != GL_DEBUG_SEVERITY_NOTIFICATION || compiler::verbose_output){
	  fprintf( stderr, "[%s] GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
				(type == GL_DEBUG_TYPE_ERROR ? "E" : "i"),
			    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
				type, severity, message );
	}
}

void gl_init(){
    glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(1, 1, "Null Window", NULL, NULL);
	if (!window)
	{
		print(PRINT_ERROR,"Failed to create the GLFW window\n");
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(vSync);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		print(PRINT_ERROR,"Failed to initialize OpenGL context");
	}
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glEnable              ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, 0 );


	if(compiler::verbose_output)
		print(PRINT_INFO,"OpenGL was loaded succesfully");
	
	const char* renderer = (char*)glGetString(GL_RENDERER);
	printf("[i] Using GPU: %s\n", renderer);
}