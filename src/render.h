#pragma once

#include "common.h"
#include <vector>

struct Vertex {
	float x, y, z;
	float r, g, b;
};

class Renderer {
	public:
		Renderer(HWND hWnd, HDC hdc, HGLRC hrc);
		~Renderer();

		void set_shader(const char *vertex, const char *fragment);
		void use_default_shader(void);
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

