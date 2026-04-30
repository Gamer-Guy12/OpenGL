#include "./window.h"
#include "Windows.h"
#include <iostream>
#include "stdlib.h"

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
		exit(-1);
	}

	if (!SetPixelFormat(hdc, iPixelFormat, &pfd)) {
		MessageBox(hWnd, "Error", "Failed to determine Pixel Format", MB_OK);
		exit(-1);
	}	

	ReleaseDC(hWnd, hdc);
}

Window::Window(const char *name, HINSTANCE hInstance, int width, int height) : hInstance(hInstance), width(width), height(height), wc({0}) {
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = name;
	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.cbWndExtra = sizeof(Window *);

	RegisterClassEx(&wc);
	std::cout << "Registered Class: " << GetLastError() << std::endl;

	hWnd = CreateWindowExA(
			WS_EX_TOPMOST,
			wc.lpszClassName,
			name,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,
			NULL,
			NULL,
			hInstance,
			NULL
			);
	std::cout << "Created Window: " << GetLastError() << std::endl;

	if (hWnd == NULL) {
		exit(-1);
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

HINSTANCE Window::get_hinstance(void) {
	return hInstance;
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

