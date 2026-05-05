#include "./shader.h"
#include <exception>
#include <iostream>

static void handle_errors(std::string msg) {
	DWORD error = GetLastError();
	std::cout << msg << ": " << error << std::endl;

	if (error) {
		system("pause");
		ExitProcess(-1);
	}
}

Shader::Shader(std::string vert, std::string frag) {
	unsigned int vertHandle;
	unsigned int fragHandle;
	const char *vert_code = vert.c_str();
	const char *frag_code = frag.c_str();
	int success;
	char infoLog[512];

	vertHandle = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertHandle, 1, &vert_code, NULL);
	glCompileShader(vertHandle);
	glGetShaderiv(vertHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertHandle, 512, NULL, infoLog);
		std::cout << "Error Log: \n" << infoLog << std::endl;
		system("pause");
 		throw std::runtime_error("Failed to compile vertex shader");
	}

	fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragHandle, 1, &frag_code, NULL);
	glCompileShader(fragHandle);
	glGetShaderiv(fragHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragHandle, 512, NULL, infoLog);
		std::cout << "Error Log: \n" << infoLog << std::endl;
		system("pause");
 		throw std::runtime_error("Failed to compile fragment shader");
	}
	
	handle = glCreateProgram();
	glAttachShader(handle, vertHandle);
	glAttachShader(handle, fragHandle);
	glLinkProgram(handle);
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(handle, 512, NULL, infoLog);
		std::cout << "Error Log: \n" << infoLog << std::endl;
		system("pause");
		throw std::runtime_error("failed to link program");
	}

	glDeleteShader(vertHandle);
	glDeleteShader(fragHandle);

	std::cout << "Successfully created shader, handle: " << handle << std::endl;
}

Shader::~Shader() {
	glDeleteProgram(handle);
}

void Shader::use(void) {
	glUseProgram(handle);
}

unsigned int Shader::get_handle(void) {
	return handle;
}
