#pragma once

#include <atomic>
#include "windows.h"
#include "./glad/wgl.h"
#include "./glad/gl.h"

typedef struct {
	HWND hWnd;
	HDC hdc;
	HGLRC hrc;
} RENDERPARAMS, *PRENDERPARAMS;

HWND getHwnd(void);
int getWidth(void);
int getHeight(void);

DWORD WINAPI AppThread(LPVOID _);
void startup(HWND hWnd, HDC hdc, HGLRC hrc);
void frame(HWND hWnd, HDC hdc, HGLRC hrc);
void cleanup(HWND hWnd, HDC hdc, HGLRC hrc);

LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnSize(HWND hWnd, UINT flag, int width, int height);

bool is_running(void);
void renderer_done(void);

