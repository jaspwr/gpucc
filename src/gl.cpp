// Copyright (C) 2023 Jasper Parker <j@sperp.dev>
//
// This file is part of Meow.
//
// Meow is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// Meow is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Meow. If not, see <https://www.gnu.org/licenses/>.


#include "gl.h"
#include "exception.h"

Shaders Gl::compile_shaders() {
	return Shaders {
		Shader("shaders/tokeniser.glsl", GL_ALL_BARRIER_BITS),
		Shader("shaders/literals.glsl", GL_ALL_BARRIER_BITS),
		Shader("shaders/ast.glsl", GL_ALL_BARRIER_BITS),
		Shader("shaders/codegen.glsl", GL_ALL_BARRIER_BITS),
		Shader("shaders/type_propagation.glsl", GL_ALL_BARRIER_BITS),
		Shader("shaders/instruction_selection.glsl", GL_ALL_BARRIER_BITS),
		Shader("shaders/liveness.glsl", GL_ALL_BARRIER_BITS),
		Shader("shaders/register_allocator.glsl", GL_ALL_BARRIER_BITS),
	};
}

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam ) {
	if(severity != GL_DEBUG_SEVERITY_NOTIFICATION){
	    fprintf( stderr, "[%s] GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			type == GL_DEBUG_TYPE_ERROR ? "E" : "i",
			type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ,
			type, severity, message );
	}
	if (type == GL_DEBUG_TYPE_ERROR) {
		throw Exception("GL Error");
	}
}


void Gl::init(bool dbg_output) {
	GLFWwindow* window;
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(1, 1, "", NULL, NULL);
	if (!window) {
		throw Exception("Failed to create the GLFW window.");
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw Exception("Failed to initialize OpenGL context");
	}
	const unsigned int SCREEN_WIDTH = 256;
	const unsigned int SCREEN_HEIGHT = 128;
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	if (dbg_output) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
	}

    const char* renderer = (char*)glGetString(GL_RENDERER);
	printf("[i] Using GPU: %s\n", renderer);
    glFinish();
}
