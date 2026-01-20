// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "sdl.hpp"

#include <cassert>
#include <stdexcept>

namespace jngl {

namespace {
bool gIssue121Workaround = false;
} // namespace

SDL::SDL() {
	if (gIssue121Workaround) {
		return; // FIXME: Workaround for https://github.com/jhasse/jngl/issues/121
	}
	gIssue121Workaround = true;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		throw std::runtime_error(SDL_GetError());
	}
	setHint(SDL_HINT_MOUSE_TOUCH_EVENTS, false);
	setHint(SDL_HINT_TOUCH_MOUSE_EVENTS, false);
}
SDL::~SDL() {
	// FIXME: Workaround for https://github.com/jhasse/jngl/issues/121
	// SDL_Quit();
}
void SDL::setHint(const char* name, bool value) {
	[[maybe_unused]] const auto result = SDL_SetHint(name, value ? "1" : "0");
	assert(result == SDL_TRUE);
}

} // namespace jngl
