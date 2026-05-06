#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include "./window.h"
#include <stdlib.h>
#include <stdio.h>
#include "./common.h"
#include "./shader.h"
#include "./renderer.h"
#include <thread>

static HMODULE libGL;
HINSTANCE instance;

HINSTANCE get_instance() {
	return instance;
}

static void* custom_gl_loader(const char* name) {
    // Try the extension loader first (for 1.2+ functions)
    void* p = (void*)wglGetProcAddress(name);
    
    // Check if wglGetProcAddress failed
    // Windows returns NULL, 0x1, 0x2, 0x3, or -1 on failure
    if (p == NULL || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
        // Fallback to the module's export table (for 1.0 and 1.1 functions)
        p = (void*)GetProcAddress(libGL, name);
    }
    
    if (p)
	    SetLastError(0);
    return p;
}

Window *window;

std::vector<Vertex> vertices;
std::vector<unsigned int> indices;

void render_thread() {
	std::string vertPath = "assets/shader.vert";
	std::string fragPath = "assets/shader.frag";

	std::ifstream vertFile(vertPath);
	std::ifstream fragFile(fragPath);
	if (!vertFile.is_open() || ! fragFile.is_open()) {
		std::cout << "Failed to open shader files\n";
		system("pause");
		ExitProcess(-1);
	}

	std::stringstream vertBuffer;
	std::stringstream fragBuffer;
	vertBuffer << vertFile.rdbuf();
	fragBuffer << fragFile.rdbuf();

	Renderer *renderer = new Renderer(window);
	Shader *shader = new Shader(vertBuffer.str(), fragBuffer.str());
	renderer->use_shader(shader);

	auto start = std::chrono::steady_clock::now();
	auto second_start = std::chrono::steady_clock::now();

	vertices.push_back({
		-0.1f, -0.5f, 0.0f,
		1.0f, 0.0f, 0.0f
	});

	vertices.push_back({
		0.1f, -0.5f, 0.0f,
		0.0f, 1.0f, 0.0f
	});

	vertices.push_back({
		-0.1f, -0.25f, 0.0f,
		0.0f, 0.0f, 1.0f
	});

	vertices.push_back({
		0.1f, -0.25f, 0.0f,
		1.0f, 1.0f, 1.0f
	});

	vertices.push_back({
		-0.05f, -0.25f, 0.0f,
		1.0f, 0.0f, 0.0f
	});

	vertices.push_back({
		0.05f, -0.25f, 0.0f,
		0.0f, 1.0f, 0.0f
	});

	vertices.push_back({
		-0.05f, 0.5f, 0.0f,
		0.0f, 0.0f, 1.0f
	});

	vertices.push_back({
		0.05f, 0.5f, 0.0f,
		1.0f, 1.0f, 1.0f
	});

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(2);

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(6);

	indices.push_back(5);
	indices.push_back(7);
	indices.push_back(6);

	renderer->upload_vertices(vertices);
	renderer->upload_indices(indices);

	while (renderer->running()) {
		renderer->draw();
		renderer->handle_life();
		auto current = std::chrono::steady_clock::now();
		auto diff = current - start;
		auto second_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - second_start);
		if (second_ms.count() >= 1000) {
			second_start = std::chrono::steady_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::microseconds>(diff);
			std::cout << 1000000 / ms.count() << " fps\n";
		}
		start = current;
	}

	delete renderer;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	AllocConsole();

	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);

	instance = hInstance;

	window = new Window("My Window", 1280, 720);
	window->show();

	std::thread render(render_thread);

	window->process();

	render.join();
	delete window;

	FreeConsole();

	return 0;
}
