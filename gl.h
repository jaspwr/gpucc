#pragma once
#include "inc.h"
#include "ui.h"
#include "libraries/include/glad/glad.h"
#include "libraries/include/GLFW/glfw3.h"
#define DEBUG_WINDOW
const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 1024;
extern GLuint screenTex;
extern GLFWwindow* window;
extern GLuint screenShaderProgram;
extern GLuint VAO, VBO, EBO;
GLuint load_to_vram(unsigned char data[],int wid, int hei, GLint internal_format, GLenum format);
GLuint create_vram_image(int data_length,  GLenum format);
void gl_init();
const GLfloat vertices[] =
{
	-1.0f, -1.0f , 0.0f, 0.0f, 0.0f,
	-1.0f,  1.0f , 0.0f, 0.0f, 1.0f,
	 1.0f,  1.0f , 0.0f, 1.0f, 1.0f,
	 1.0f, -1.0f , 0.0f, 1.0f, 0.0f,
};
const GLuint indices[] =
{
	0, 2, 1,
	0, 3, 2
};
