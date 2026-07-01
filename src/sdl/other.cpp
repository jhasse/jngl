// Copyright 2021-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Finally.hpp"
#include "../jngl/other.hpp"

#include <SDL3/SDL.h>

#ifdef JNGL_VULKAN
#include "../Renderer.hpp"
#include "../vulkan/VulkanRenderer.hpp"
#endif

namespace jngl {

std::string getPreferredLanguage() {
#ifdef __EMSCRIPTEN__
	if (const auto lang_cstr = std::getenv("LANG")) {
		const std::string lang = lang_cstr;
		if (lang.size() >= 2) {
			return lang.substr(0, 2);
		}
	}
#else
	int count = 0;
	SDL_Locale** locales = SDL_GetPreferredLocales(&count);
	Finally freeLocales([locales]() { SDL_free(static_cast<void*>(locales)); });
	for (int i = 0; i < count; ++i) {
		if (locales[i] && locales[i]->language && locales[i]->language[0] != '\0' &&
		    locales[i]->language[1] != '\0' && locales[i]->language[2] == '\0') {
			return locales[i]->language;
		}
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
#ifdef JNGL_VULKAN
	static_cast<VulkanRenderer&>(getRenderer()).setVerticalSync(enabled);
#else
	SDL_GL_SetSwapInterval(enabled ? 1 : 0);
#endif
}

bool getVerticalSync() {
#ifdef JNGL_VULKAN
	return static_cast<VulkanRenderer&>(getRenderer()).getVerticalSync();
#else
	int result;
	if (!SDL_GL_GetSwapInterval(&result)) {
		throw std::runtime_error(SDL_GetError());
	}
	return result == 1;
#endif
}

} // namespace jngl
