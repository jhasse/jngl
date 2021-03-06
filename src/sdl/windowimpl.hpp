// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "sdl.hpp"

#if defined(__has_include) && __has_include(<optional>)
#include <optional>
using std::optional;
using std::nullopt; // NOLINT
#else
#include <experimental/optional>
using std::experimental::optional;
using std::experimental::nullopt;
#endif

namespace jngl {

class WindowImpl {
public:
	WindowImpl() = default;
	~WindowImpl() {
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(sdlWindow);
	}
	WindowImpl(const WindowImpl&) = delete;
	WindowImpl& operator=(const WindowImpl&) = delete;
	WindowImpl(WindowImpl&&) = delete;
	WindowImpl& operator=(WindowImpl&&) = delete;

	SDL_Window* sdlWindow = nullptr;
	SDL_GLContext context = nullptr;
	optional<SDL_FingerID> currentFingerId;
};

} // namespace jngl
