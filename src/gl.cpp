#include "gl.h"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
	if(severity != GL_DEBUG_SEVERITY_NOTIFICATION){
	    fprintf( stderr, "[%s] GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			type == GL_DEBUG_TYPE_ERROR ? "E" : "i",
			type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ,
			type, severity, message );
	}
}


void Gl::init() {
	GLFWwindow* window;
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(1, 1, "Null Window", NULL, NULL);
	if (!window)
	{
		throw "Failed to create the GLFW window.";
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw "Failed to initialize OpenGL context";
	}
	const unsigned int SCREEN_WIDTH = 256;
	const unsigned int SCREEN_HEIGHT = 128;
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

    const char* renderer = (char*)glGetString(GL_RENDERER);
	printf("[i] Using GPU: %s\n", renderer);
    glFinish();
}