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

Window *window;

std::vector<Vertex> vertices;
std::vector<unsigned int> indices;

HINSTANCE get_instance() {
	return instance;
}

std::string get_relative_path(std::string path) {
	char current_file[128];
	GetModuleFileNameA(nullptr, current_file, 128);
	std::string current_file_s = current_file;
	const size_t last_slash_pos = current_file_s.rfind('\\');
	std::string dir_name = current_file_s.substr(0, last_slash_pos);

	return dir_name + path;
}

void render_thread() {
	std::string vertPath = get_relative_path("/assets/shader.vert");
	std::string fragPath = get_relative_path("/assets/shader.frag");

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

	glHandle handle = renderer->load_texture(get_relative_path("/assets/baby.png"));
	renderer->use_shader(shader);
	renderer->use_texture(handle, "texture0");

	auto start = std::chrono::steady_clock::now();
	auto second_start = std::chrono::steady_clock::now();

	vertices.push_back({
		-0.7f, -0.7f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f
	});

	vertices.push_back({
		0.7f, -0.7f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f
	});

	vertices.push_back({
		-0.7f, 0.7f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f
	});

	vertices.push_back({
		0.7f, 0.7f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f
	});

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(2);

	renderer->upload_vertices(vertices);
	renderer->upload_indices(indices);
	int count = 0;

	while (renderer->running()) {
		auto current = std::chrono::steady_clock::now();
		auto diff = current - start;
		auto second_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - second_start);
		if (second_ms.count() >= 1000) {
			second_start = std::chrono::steady_clock::now();
			auto ms = std::chrono::duration_cast<std::chrono::microseconds>(diff);
			std::cout << 1000000 / ms.count() << " fps\n";
		}
		start = current;

		renderer->start_render();
		renderer->draw();
		renderer->end_render();

		renderer->handle_life();
	}

	delete renderer;
}

void key_handler(int event, void *data, void *param) {
	if (event != WKDOWN) return;

	Window *window = (Window *)data;

	if (window->get_key() == VK_UP) {
		std::cout << "Pressed Up!\n";
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	AllocConsole();

	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);

	instance = hInstance;

	window = new Window("My Window", 1280, 720);
	window->show();
	window->subscribe_events(key_handler, nullptr);

	std::thread render(render_thread);

	window->process();

	render.join();
	delete window;

	FreeConsole();

	return 0;
}
