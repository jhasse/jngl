#pragma once

#include "sdl.hpp"

#ifdef __APPLE__
#include <experimental/optional>
using std::experimental::optional;
using std::experimental::nullopt;
#else
#include <optional>
using std::optional;
using std::nullopt;
#endif

namespace jngl {

class WindowImpl {
public:
	SDL_Window* sdlWindow;
	SDL_GLContext context;
	optional<SDL_FingerID> currentFingerId;
};

} // namespace jngl
