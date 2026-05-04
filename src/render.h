#pragma once

#include "./common.h"
#include "./window.h"
#include "./shader.h"
#include <vector>

struct Vertex {
	float x, y, z;
	float r, g, b;
};

class Renderer {
	public:
		Renderer(Window &window, Shader &shader);
		~Renderer();

		void upload_vertices(std::vector<Vertex> &vertices);
		void upload_indices(std::vector<int> &indices);

		void draw(void);

	private:
		HWND hWnd;
		HDC hdc;
		HGLRC hrc;

		std::vector<Vertex> vertices;
		std::vector<int> indices;
};

