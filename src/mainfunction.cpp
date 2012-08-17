#include "jngl.hpp"
#include "main.hpp"

std::function<void()> mainInit;

int main(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		jngl::args.push_back(argv[i]);
	}
	jngl::mainInit();
	if (jngl::running()) {
		jngl::mainLoop();
	}
}