// Copyright 2007-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../window.hpp"

#include <fontconfig/fontconfig.h>
#include <stdexcept>

namespace jngl {

std::string Window::GetFontFileByName(const std::string& fontname) {
	const double size = 12;
	FcFontSet* fontSet = FcFontSetCreate();

	FcPattern* pattern = FcPatternBuild(nullptr, FC_FAMILY, FcTypeString, fontname.c_str(), FC_SIZE,
	                                    FcTypeDouble, size, nullptr);
	FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
	FcDefaultSubstitute(pattern);

	FcResult result;
	FcPattern* match = FcFontMatch(nullptr, pattern, &result);
	FcPatternDestroy(pattern);

	if (!match) {
		FcFontSetDestroy(fontSet);
		throw std::runtime_error(std::string("Couldn't load " + fontname));
	}
	FcFontSetAdd(fontSet, match);

	if (fontSet->nfont == 0) {
		throw std::runtime_error(std::string("Couldn't load " + fontname));
	}

	FcChar8* filename = nullptr;
	FcPatternGetString(fontSet->fonts[0], FC_FILE, 0, &filename);
	// FcChar8 is unsigned, that's why we need the cast here:
	return reinterpret_cast<const char*>(filename); // NOLINT
}

} // namespace jngl
