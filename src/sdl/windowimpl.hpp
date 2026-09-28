// Copyright 2020-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "../Renderer.hpp"
#include "sdl.hpp"

#include <memory>

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
		// Tear down the rendering backend (and, on Vulkan, its surface/swapchain/instance) before
		// the window and context it renders to.
		renderer.reset();
#ifndef JNGL_VULKAN
		SDL_GL_DestroyContext(context);
#endif
		SDL_DestroyWindow(sdlWindow);
	}
	WindowImpl(const WindowImpl&) = delete;
	WindowImpl& operator=(const WindowImpl&) = delete;
	WindowImpl(WindowImpl&&) = delete;
	WindowImpl& operator=(WindowImpl&&) = delete;

	SDL_Window* sdlWindow = nullptr;
	SDL_GLContext context = nullptr;
	std::unique_ptr<Renderer> renderer;
	optional<SDL_FingerID> currentFingerId;

	/// If the window is resized we save the actual window size here for mouse input to work:
	float actualWidth;
	float actualHeight;
	int actualCanvasWidth;
	int actualCanvasHeight;
	SDL_SystemCursor cursor = SDL_SYSTEM_CURSOR_DEFAULT;
	SDL_SystemCursor currentCursor = cursor;
	std::unique_ptr<SDL_Cursor, void (*)(SDL_Cursor*)> sdlCursor{ nullptr, SDL_DestroyCursor };

	/// For Retina screens on macOS SDL does its own scaling of mouse coordinates, etc. :(
	float hidpiScaleFactor;
};

} // namespace jngl
