cmake_minimum_required(VERSION 4.1)

project(App)

add_executable(App WIN32)
target_link_libraries(App user32 kernel32 gdi32 advapi32 opengl32)

target_sources(App
	PRIVATE
		src/startup.cpp
		src/app.cpp
		src/gl.c
		src/wgl.c
		src/renderer.cpp
		src/window.cpp
)

target_include_directories(App PRIVATE include)
