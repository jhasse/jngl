#pragma once

#include "sdl.hpp"

#if defined(__has_include) && __has_include(<optional>)
#include <optional>
using std::optional;
using std::nullopt;
#else
#include <experimental/optional>
using std::experimental::optional;
using std::experimental::nullopt;
#endif

namespace jngl {

class WindowImpl {
public:
	SDL_Window* sdlWindow;
	SDL_GLContext context;
	optional<SDL_FingerID> currentFingerId;
};

} // namespace jngl
