// Copyright 2021-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#include <SDL.h>

#if defined(__has_include) && __has_include(<SDL_locale.h>)
#include <SDL_locale.h>
#endif

namespace jngl {

std::string getPreferredLanguage() {
#if defined(__has_include) && __has_include(<SDL_locale.h>)
	SDL_Locale* locale = SDL_GetPreferredLocales();
	if (locale && locale->language && locale->language[0] != '\0' && locale->language[1] != '\0' &&
	    locale->language[2] == '\0') {
		return locale->language;
	}
#endif
	return "en";
}

void openURL(const std::string& url) {
#if SDL_VERSION_ATLEAST(2, 0, 14)
	SDL_OpenURL(url.c_str());
#endif
}

void setVerticalSync(bool enabled) {
	SDL_GL_SetSwapInterval(enabled ? 1 : 0);
}

bool getVerticalSync() {
	return SDL_GL_GetSwapInterval() == 1;
}

} // namespace jngl
