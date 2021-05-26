// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#include <SDL.h>

namespace jngl {

std::string getPreferredLanguage() {
	SDL_Locale* locale = SDL_GetPreferredLocales();
	if (locale && locale->language && locale->language[0] != '\0' && locale->language[1] != '\0' &&
	    locale->language[2] == '\0') {
		return locale->language;
	}
	return "en";
}

} // namespace jngl
