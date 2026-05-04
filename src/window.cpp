#include "./window.h"
#include "Windows.h"
#include <iostream>
#include <atomic>

void handle_errors(std::string msg) {
	DWORD error = GetLastError();
	std::cout << msg << ": " << error << std::endl;

	if (error) {
		ExitProcess(-1);
	}
}

LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Window *window = (Window *)GetWindowLongPtrA(hWnd, 0);
	static PAINTSTRUCT ps;

	switch (uMsg) {
		case WM_PAINT:
			{
				BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			}
			return 0;
		case WM_SIZE:
			{
				int width = LOWORD(lParam);
				int height = HIWORD(lParam);
				window->resize(width, height);
			}
			return 0;
		case WM_DESTROY:
			delete window;
			PostQuitMessage(0);
			return 0;
		case WM_ERASEBKGND:
			return 1;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void SetPixelFormat(HWND hWnd) {
	HDC hdc = GetDC(hWnd);

	const PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		8,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int iPixelFormat = ChoosePixelFormat(hdc, &pfd);

	if (iPixelFormat == 0) {
		MessageBox(hWnd, "Error", "Failed to determine Pixel Format", MB_OK);
		ExitProcess(-1);
	}

	if (!SetPixelFormat(hdc, iPixelFormat, &pfd)) {
		MessageBox(hWnd, "Error", "Failed to determine Pixel Format", MB_OK);
		ExitProcess(-1);
	}	

	ReleaseDC(hWnd, hdc);
}

std::atomic<int> classStatus(0);
WNDCLASSEXA wc = {0};

void setup_class(void) {
	int old = 0;
	if (!classStatus.compare_exchange_strong(old, 1)) {
		while (classStatus != 2) {}
		return;
	}

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = get_instance();
	wc.lpszClassName = "StandardClass";
	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.cbWndExtra = sizeof(Window *);
	RegisterClassEx(&wc);
	handle_errors("Registered Class");

	classStatus = 2;
}

Window::Window(const char *name, int width, int height) : width(width), height(height) {
	setup_class();

	hWnd = CreateWindowExA(
			0,
			wc.lpszClassName,
			name,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,
			NULL,
			NULL,
			get_instance(),
			NULL
			);
	handle_errors("Created Window");

	if (hWnd == NULL) {
		ExitProcess(-1);
	}

	SetPixelFormat(hWnd);
	SetWindowLongPtrA(hWnd, 0, (LONG_PTR)this);

	hdc = GetDC(hWnd);
}

Window::~Window() {
	ReleaseDC(hWnd, hdc);
	DestroyWindow(hWnd);
}

void Window::show(void) {
	ShowWindow(hWnd, SW_SHOW);
}

void Window::process(void) {
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool Window::running(void) {
	return is_running;
}

void Window::subscribe_events(std::function<void(int event, void *data)> handler) {
	handlers.push_back(handler);
}

void Window::dispatch_event(int event, void *data) {
	for (auto handler : handlers) {
		handler(event, data);
	}
}

HWND Window::get_hwnd(void) {
	return hWnd;
}

HDC Window::get_hdc(void) {
	return hdc;
}

void Window::resize(int _width, int _height) {
	width = _width;
	height = _height;
}

