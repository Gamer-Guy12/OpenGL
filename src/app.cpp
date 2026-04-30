#include "render.h"
#include <iostream>

Renderer *renderer;

void startup(HWND hWnd, HDC hdc, HGLRC hrc) {
	renderer = new Renderer(hWnd, hdc, hrc);
}

void frame(HWND hWnd, HDC hdc, HGLRC hrc) {
	renderer->draw();
}

void cleanup(HWND hWnd, HDC hdc, HGLRC hrc) {
	delete renderer;
}

