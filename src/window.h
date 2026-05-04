#pragma once

#include "./common.h"
#include <vector>
#include <functional>

#define WRESIZE 0
#define WCLOSE 1

class Window {
	public:
		Window(const char *name, int width, int height);
		~Window();

		void show(void);
		void process(void);
		bool running(void);
		void resize(int width, int height);
		void subscribe_events(std::function<void(int event, void *data)>);

		HINSTANCE get_hinstance(void);
		HWND get_hwnd(void);
		HDC get_hdc(void);

	private:
		int width;
		int height;
		std::vector<std::function<void(int event, void *data)>> handlers;
		bool is_running;

		HWND hWnd;
		HDC hdc;

		void dispatch_event(int event, void *data);
};
