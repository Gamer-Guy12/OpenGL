#include "./common.h"
#include <string>

class Shader {
	public:
		Shader(std::string vert, std::string frag);
		~Shader();
		void use(void);
		unsigned int get_handle(void);

	private:
		unsigned int handle;
};
