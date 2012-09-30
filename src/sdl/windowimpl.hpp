#pragma once

namespace jngl {
	class WindowImpl {
	public:
		SDL_Window* sdlWindow;
		SDL_GLContext context;
	};
}