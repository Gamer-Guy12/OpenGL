cmake_minimum_required(VERSION 4.1)

project(App)

add_executable(App WIN32)
target_link_libraries(App user32 kernel32 gdi32 advapi32 opengl32)

target_sources(App
	PRIVATE
		src/graphics.cpp
		src/gl.c
		src/wgl.c
		src/renderer.cpp
		src/window.cpp
		src/shader.cpp
)

target_include_directories(App PRIVATE include)

add_custom_target(copy_assets ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/assets
	${CMAKE_CURRENT_BINARY_DIR}/assets
)

add_dependencies(App copy_assets)
