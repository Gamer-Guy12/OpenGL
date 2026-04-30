#include "render.h"
#include <iostream>

Renderer::Renderer(HWND hWnd, HDC hdc, HGLRC hrc) : hWnd(hWnd), hdc(hdc), hrc(hrc) {
}

Renderer::~Renderer() {
}

void Renderer::set_shader(const char *vertex, const char *fragment) {
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Failed to Compile Vertex Shader: " << infoLog << std::endl;
	}
}

void Renderer::use_default_shader(void) {
	const char *vertex = 
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"out vec4 vertexColor;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(aPos, 1.0);\n"
		"vertexColor = vec4(aColor, 1.0);\n;"
		"}\n";

	const char *fragment = 
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec4 vertexColor;\n"
		"void main()\n"
		"{\n"
		"FragColor = vertexColor;\n"
		"}\n";

	set_shader(vertex, fragment);
}

void Renderer::upload_vertices(std::vector<Vertex> &_vertices) {
	vertices.insert(vertices.end(), _vertices.begin(), _vertices.end());
}

void Renderer::upload_indices(std::vector<int> &_indices) {
	indices.insert(indices.end(), _indices.begin(), _indices.end());
}


void Renderer::draw(void) {
	glClearColor(0.4f, 0.5f, 0.6f, 1.0f);

	SwapBuffers(hdc);
	glClear(GL_COLOR_BUFFER_BIT);
}
