#pragma once

#include "./common.h"
#include <string>

class Shader {
	public:
		Shader(std::string vert, std::string frag);
		~Shader();
		void use(void);
		unsigned int get_handle(void);

		void setInt(std::string name, int value);
		void setUint(std::string name, unsigned int value);
		void setFloat(std::string name, float value);
		void setVec2(std::string name, float value0, float value1);
		void setVec3(std::string name, float value0, float value1, float value2);
		void setVec4(std::string name, float value0, float value1, float value2, float value3);

	private:
		unsigned int handle;
};
