#include "gl.h"



const char* screenVertexShaderSource = R"(#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uvs;
out vec2 UVs;
void main()
{
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	UVs = uvs;
})";
const char* screenFragmentShaderSource = R"(#version 460 core
out vec4 FragColor;
uniform sampler2D screen;
in vec2 UVs;
void main()
{
	FragColor = texture(screen, UVs);
})";


const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

GLFWwindow* window;

GLuint screenTex;
GLuint screenShaderProgram;
GLuint VAO, VBO, EBO;

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

// void get_from_vram(GLuint img, int length, unsigned char (*data)[8000]){
// 	glReadPixels(0,0,length,1,GL_RGBA,GL_UNSIGNED_BYTE,data);
// }

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
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

void gl_init(){
    glfwInit();

//#ifdef DEBUG_WINDOW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ahsdkjhaskdljahskdasjkh", NULL, NULL);
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
//#endif

	glEnable              ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( MessageCallback, 0 );


	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);
	glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);

	glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
	glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTex, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);


	GLuint screenVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(screenVertexShader, 1, &screenVertexShaderSource, NULL);
	glCompileShader(screenVertexShader);
	GLuint screenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(screenFragmentShader, 1, &screenFragmentShaderSource, NULL);
	glCompileShader(screenFragmentShader);
	screenShaderProgram = glCreateProgram();
	glAttachShader(screenShaderProgram, screenVertexShader);
	glAttachShader(screenShaderProgram, screenFragmentShader);
	glLinkProgram(screenShaderProgram);
	glDeleteShader(screenVertexShader);
	glDeleteShader(screenFragmentShader);


    print(PRINT_INFO,"OpenGL was loaded succesfully");
	
	//const char* vendor = (char*)glGetString(GL_VENDOR); // Returns the vendor
	const char* renderer = (char*)glGetString(GL_RENDERER); // Returns a hint to the model
	//strcat(s, vendor);
	//strcat(s, renderer);
	printf("[i] Using GPU: %s\n", renderer);
	//print(PRINT_INFO, renderer);


}