#pragma once

#include "./common.h"
#include <vector>
#include <deque>
#include <functional>

// Window Resize
#define WRESIZE 0
// Window Close
#define WCLOSE 1
// Window Key Down
#define WKDOWN 2
// Window Key Up
#define WKUP 3

class Window {
	public:
		Window(const char *name, int width, int height);
		~Window();

		void show(void);
		void process(void);
		bool running(void);
		void resize(int width, int height);
		void destroy(void);
		int subscribe_events(std::function<void(int event, void *data, void *param)>, void *param);
		void unsubscribe_events(int handler);

		uint64_t get_key(void);

		HWND get_hwnd(void);
		HDC get_hdc(void);

		int get_width(void);
		int get_height(void);

		friend LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);

	private:
		int width;
		int height;
		std::vector<std::function<void(int event, void *data, void *param)>> handlers;
		std::vector<void *> params;
		std::deque<uint64_t> keys;
		bool is_running;

		HWND hWnd;
		HDC hdc;

		void dispatch_event(int event, void *data);
		void add_key(uint64_t key);
};
