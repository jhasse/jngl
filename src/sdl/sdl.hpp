// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../jngl/Singleton.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#endif
#include <SDL3/SDL.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace jngl {

/// Calls SDL_Init without letting SDL take over SIGINT
///
/// SDL only installs its SIGINT handler (which turns Ctrl+C into SDL_EVENT_QUIT) if the current one
/// is SIG_DFL. Window::mainLoop installs JNGL's own handler, outside of it Ctrl+C should simply
/// kill the process.
void sdlInit(SDL_InitFlags);

class SDL : public Singleton<SDL> {
	friend class Singleton<SDL>;

public:
	SDL(const SDL&) = delete;
	SDL& operator=(const SDL&) = delete;
	SDL(SDL&&) = delete;
	SDL& operator=(SDL&&) = delete;

private:
	SDL();
	~SDL();
	static void setHint(const char* name, bool);
};

} // namespace jngl
