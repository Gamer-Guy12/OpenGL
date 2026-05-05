#include "renderer.h"
#include <iostream>
#include <atomic>

static HMODULE libGL;

static void handle_errors(std::string msg) {
	DWORD error = GetLastError();
	std::cout << msg << ": " << error << std::endl;

	if (error) {
		system("pause");
		ExitProcess(-1);
	}
}

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

static std::atomic<int> libgl_state(0);

static void handle_libgl() {
    int old = 0;
    if (!libgl_state.compare_exchange_strong(old, 1)) {
        while (libgl_state != 2) {}
        return;
    }

    libGL = LoadLibraryA("opengl32.dll");
    if (!libGL) {
        std::cout << "Failed to load OpenGL Library\n";
		system("pause");
        ExitProcess(-1);
    }
}

Renderer::Renderer(Window *window) : hWnd(window->get_hwnd()), hdc(window->get_hdc()), window(window)
{
    handle_libgl();

    HGLRC hrc1 = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hrc1);
    std::cout << "Made First Context: " << GetLastError() << std::endl;

    if (!gladLoadGL((GLADloadfunc)custom_gl_loader))
    {
        handle_errors("Failed to make glad context");
    }

    const char *version = (const char *)glGetString(GL_VERSION);
    if (version)
    {
        std::cout << "Hardware supports: " << version << std::endl;
    }
    else
    {
        std::cout << "Error Checking Version: " << version << std::endl;
    }

    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        GL_CONTEXT_PROFILE_MASK, GL_CONTEXT_CORE_PROFILE_BIT,
        0};

    HGLRC hrc = ((PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB"))(hdc, 0, attribs);
    handle_errors("Created Context");
    wglMakeCurrent(hdc, hrc);
    wglDeleteContext(hrc1);
    glViewport(0, 0, window->get_width(), window->get_height());
}

Renderer::~Renderer()
{
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(hrc);
}

void Renderer::use_shader(Shader *shader) {
    this->shader = shader;
}

void Renderer::draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.7f, 0.6f, 0.5f, 1.0f);

    SwapBuffers(hdc);
}