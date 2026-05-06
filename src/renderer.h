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
		Renderer(Window *window);
		~Renderer();

		void use_shader(Shader *shader);
		void clear_data(void);
		void upload_vertices(std::vector<Vertex> &vertices);
		void upload_indices(std::vector<unsigned int> &indices);
		void set_background(float r, float g, float b);

		void draw(void);
		bool running(void);
		void handle_life(void);

	private:
		friend void handle_resize(int event, void *data, void *param);
		friend void handle_destroy(int event, void *data, void *param);

		void draw_triangles(void);

		HWND hWnd;
		HDC hdc;
		HGLRC hrc;

		// 0 = Running
		// 1 = Closing
		// 2 = Dead
		int state = 0;
		int resize_handler;
		int close_handler;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		Window *window;
		Shader *shader = nullptr;

		bool resize = false;
		int newWidth;
		int newHeight;

		float br = 0.5f;
		float bg = 0.6f;
		float bb = 0.8f;

		unsigned int vao, vbo, ebo;
		bool reload = false;
};

