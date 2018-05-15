#pragma once

#include "sdl.hpp"

#include <optional>

namespace jngl {
	class WindowImpl {
	public:
		SDL_Window* sdlWindow;
		SDL_GLContext context;
		std::optional<SDL_FingerID> currentFingerId;
	};
}
