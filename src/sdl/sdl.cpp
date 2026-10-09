// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "sdl.hpp"

#include <cassert>
#include <csignal>
#include <stdexcept>

namespace jngl {

namespace {
bool gIssue121Workaround = false;
} // namespace

void sdlInit(const SDL_InitFlags flags) {
#ifndef __EMSCRIPTEN__
	const auto previousSigintHandler = std::signal(SIGINT, SIG_IGN);
#endif
	const bool success = SDL_Init(flags);
#ifndef __EMSCRIPTEN__
	std::signal(SIGINT, previousSigintHandler);
#endif
	if (!success) {
		throw std::runtime_error(SDL_GetError());
	}
}

SDL::SDL() {
	if (gIssue121Workaround) {
		return; // FIXME: Workaround for https://github.com/jhasse/jngl/issues/121
	}
	gIssue121Workaround = true;
	sdlInit(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
	setHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, true);
	setHint(SDL_HINT_MOUSE_TOUCH_EVENTS, false);
	setHint(SDL_HINT_TOUCH_MOUSE_EVENTS, false);
}
SDL::~SDL() {
	// FIXME: Workaround for https://github.com/jhasse/jngl/issues/121
	// SDL_Quit();
}
void SDL::setHint(const char* name, bool value) {
	[[maybe_unused]] const auto result = SDL_SetHint(name, value ? "1" : "0");
	assert(result);
}

} // namespace jngl
