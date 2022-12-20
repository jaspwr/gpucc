#pragma once
#include "inc.h"
#include "ui.h"
//#include "libraries/include/glad/glad.h"
//#include "libraries/include/GLFW/glfw3.h"
#define DEBUG_WINDOW
const unsigned int SCREEN_WIDTH = 256;
const unsigned int SCREEN_HEIGHT = 128;
GLuint load_to_vram(unsigned char data[],int wid, int hei, GLint internal_format, GLenum format);
GLuint create_vram_image(int data_length,  GLenum format);
GLuint create_ssbo(int size);
GLuint load_to_ssbo(int size, int type_size, int* data);
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
