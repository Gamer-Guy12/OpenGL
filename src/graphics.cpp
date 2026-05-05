#include <iostream>
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

	while (renderer->running()) {
		renderer->draw();
		renderer->handle_life();
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
