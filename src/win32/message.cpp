// Copyright 2009-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/message.hpp"

#include "unicode.hpp"

#include <string>
#include <iostream>
#include <windows.h>

#ifdef JNGL_UWP
#include <SDL.h>
#endif

namespace jngl {

#ifdef JNGL_UWP
void errorMessage(const std::string& text) {
	printMessage(text + "\n");
	// TODO
}
#endif

void printMessage(const std::string& text) {
#ifdef JNGL_UWP
	SDL_Log("%s", text.c_str());
#else
	std::cout << text << std::flush;
#endif
}

} // namespace jngl
