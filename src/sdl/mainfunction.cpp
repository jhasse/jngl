#include "../jngl.hpp"
#include "../main.hpp"

int main(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		jngl::args.push_back(argv[i]);
	}
	if (jngl::running()) {
		jngl::mainLoop();
	}
}