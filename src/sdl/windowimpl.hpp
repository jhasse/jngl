// Copyright 2020-2023 Jan Niklas Hasse <jhasse@bixense.com>
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
		SDL_GL_DestroyContext(context);
		SDL_DestroyWindow(sdlWindow);
	}
	WindowImpl(const WindowImpl&) = delete;
	WindowImpl& operator=(const WindowImpl&) = delete;
	WindowImpl(WindowImpl&&) = delete;
	WindowImpl& operator=(WindowImpl&&) = delete;

	SDL_Window* sdlWindow = nullptr;
	SDL_GLContext context = nullptr;
	optional<SDL_FingerID> currentFingerId;

	/// UWP windows can be resized, no way around. So we save the actual window size here for mouse
	/// input to work:
	float actualWidth;
	float actualHeight;
	int actualCanvasWidth;
	int actualCanvasHeight;

	/// For Retina screens on macOS SDL does its own scaling of mouse coordinates, etc. :(
	float hidpiScaleFactor;

	/// On UWP there's a bug that sometimes there are "wrong" resize events at startup (maybe due to
	/// fullscreen mode?). Just skipping them until the first frame is drawn is a workaround.
	bool firstFrame = true;
};

} // namespace jngl
