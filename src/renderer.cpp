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

typedef void (*PFNGLSWAPINTERVALEXTPROC)(int interval);

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
    PFNGLSWAPINTERVALEXTPROC glSwapIntervalEXT = ((PFNGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT"));
    // Handle Swaps
    glSwapIntervalEXT(0);

    resize_handler = window->subscribe_events(handle_resize, this);
    close_handler = window->subscribe_events(handle_destroy, this);

    clear_data();

    // Create buffer objects
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
}

// Used to delete final things
Renderer::~Renderer()
{
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(hrc);
}

void Renderer::use_shader(Shader *shader) {
    this->shader = shader;
}

void Renderer::draw_triangles() {
    glBindVertexArray(vao);

    if (reload) {
        reload = false;

        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);
    }

    glDrawArrays(GL_TRIANGLES, 0, indices.size());
}

void Renderer::draw() {
    if (state != 0) return;

    if (resize) {
        resize = false;
        glViewport(0, 0, newWidth, newHeight);
        std::cout << "Resize: " << newWidth << ", " << newHeight << std::endl;
    }

    shader->use();

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(br, bg, bb, 1.0f);

    if (indices.size() > 0 || reload) {
        draw_triangles();
    }

    SwapBuffers(hdc);
}

bool Renderer::running() {
    return state != 2;
}

// Used to handle window closing
void Renderer::handle_life() {
    if (state != 1) return;

    window->unsubscribe_events(resize_handler);
    window->unsubscribe_events(close_handler);
    state = 2;
}

void handle_resize(int event, void *data, void *param) {
    if (event != WRESIZE) return;

    Renderer *renderer = (Renderer *)param;
    Window *window = (Window *)data;

    renderer->resize = true;
    renderer->newWidth = window->get_width();
    renderer->newHeight = window->get_height();
}

void handle_destroy(int event, void *data, void *param) {
    if (event != WCLOSE) return;

    Renderer *renderer = (Renderer *)param;
    Window *window = (Window *)data;

    renderer->state = 1;
}

void Renderer::clear_data() {
    vertices.clear();
    indices.clear();
}

void Renderer::upload_vertices(std::vector<Vertex> &vertices) {
    this->vertices.insert(this->vertices.end(), vertices.begin(), vertices.end());
    reload = true;
}

void Renderer::upload_indices(std::vector<unsigned int> &indices) {
    this->indices.insert(this->indices.end(), indices.begin(), indices.end());
    reload = true;
}

void Renderer::set_background(float r, float g, float b) {
    br = r;
    bg = g;
    bb = b;
}