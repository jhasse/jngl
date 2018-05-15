#pragma once

#include "sdl.hpp"

#include <boost/optional.hpp>

namespace jngl {
	class WindowImpl {
	public:
		SDL_Window* sdlWindow;
		SDL_GLContext context;
		boost::optional<SDL_FingerID> currentFingerId;
	};
}
