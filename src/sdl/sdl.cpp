// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "sdl.hpp"

#include <cassert>
#include <stdexcept>

namespace jngl {

#ifndef __EMSCRIPTEN__
volatile std::sig_atomic_t gGotSigint = 0;
#endif

namespace {
bool gIssue121Workaround = false;
} // namespace

#ifndef __EMSCRIPTEN__
extern "C" void jngl_on_sigint(int) {
	gGotSigint = 1;
}
#endif

SDL::SDL() {
	if (gIssue121Workaround) {
		return; // FIXME: Workaround for https://github.com/jhasse/jngl/issues/121
	}
	gIssue121Workaround = true;
#ifndef __EMSCRIPTEN__
	// Install before SDL_Init so SDL leaves SIGINT alone (it only replaces SIG_DFL). Window close
	// and minimize still come through as SDL_EVENT_QUIT / SDL_EVENT_WINDOW_MINIMIZED.
	std::signal(SIGINT, jngl_on_sigint);
#endif
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
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
	assert(result);
}

} // namespace jngl
