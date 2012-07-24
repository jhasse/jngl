/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl.hpp"

#include <windows.h>

#if (defined(NDEBUG) && defined(_MSC_VER))
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#else
int main(int argc, char** argv) {
#endif
	jngl::main();
	if (jngl::running()) {
		jngl::mainLoop();
	}
}