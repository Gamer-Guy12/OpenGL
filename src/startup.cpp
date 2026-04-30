#include <iostream>
#include "./window.h"
#include "stdlib.h"
#include "./common.h"

static HMODULE libGL;

void* custom_gl_loader(const char* name) {
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	Window *window = new Window("My Window", hInstance, 1280, 720);
	window->show();
	std::cout << "Showed Window: " << GetLastError() << std::endl;
	
	libGL = LoadLibraryA("opengl32.dll");
	if (!libGL) {
		std::cout << "Failed to load OpenGL Library\n";
	}

	HDC hdc = window->get_hdc();	
	HGLRC hrc1 = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc1);
	std::cout << "Made First Context: " << GetLastError() << std::endl;

	if (!gladLoadGL((GLADloadfunc)custom_gl_loader)) {
		std::cout << "Failed to init GLAD: " << GetLastError() << std::endl;
		return -1;
	}

	const char *version = (const char*)glGetString(GL_VERSION);
	if (version) {
		std::cout << "Hardware supports: " << version << std::endl;
	} else {
		std::cout << "Error Checking Version: " << version << std::endl;
	}

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		GL_CONTEXT_PROFILE_MASK,  GL_CONTEXT_CORE_PROFILE_BIT,
		0
	};


	HGLRC hrc = ((PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB"))(hdc, 0, attribs);
	std::cout << "Created Context: " << GetLastError() << std::endl;
	wglDeleteContext(hrc1);
	wglMakeCurrent(hdc, hrc);
	wglMakeCurrent(nullptr, nullptr);

	while (window->running()) {
		window->process();
	}
	
	wglDeleteContext(hrc);
	delete window;

	return 0;
}
