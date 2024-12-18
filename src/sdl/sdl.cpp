// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "sdl.hpp"

#include <cassert>
#include <stdexcept>

namespace jngl {

SDL::SDL() {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
		throw std::runtime_error(SDL_GetError());
	}
	setHint(SDL_HINT_MOUSE_TOUCH_EVENTS, false);
	setHint(SDL_HINT_TOUCH_MOUSE_EVENTS, false);
}
SDL::~SDL() {
	SDL_Quit();
}
void SDL::setHint(const char* name, bool value) {
	[[maybe_unused]] const auto result = SDL_SetHint(name, value ? "1" : "0");
	assert(result);
}

} // namespace jngl
