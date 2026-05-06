#pragma once

#include "windows.h"
#include "./glad/wgl.h"
#include "./glad/gl.h"
#include <string>

HINSTANCE get_instance();
std::string get_relative_path(std::string path);
