/*
Copyright 2007-2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../window.hpp"

#include <fontconfig/fontconfig.h>

namespace jngl {
	std::string Window::GetFontFileByName(const std::string& fontname) {
		const double size = 12;
		FcFontSet* fontSet = FcFontSetCreate();

		FcPattern* pattern = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, fontname.c_str(),
		                                    FC_SIZE, FcTypeDouble, size, NULL);
		FcConfigSubstitute(NULL,pattern, FcMatchPattern);
		FcDefaultSubstitute(pattern);

		FcResult result;
		FcPattern* match = FcFontMatch(NULL, pattern, &result);
		FcPatternDestroy(pattern);

		if(!match)
		{
			FcFontSetDestroy(fontSet);
			throw std::runtime_error(std::string("Couldn't load " + fontname));
		}
		FcFontSetAdd(fontSet, match);

		if(fontSet->nfont == 0)
		{
			throw std::runtime_error(std::string("Couldn't load " + fontname));
		}

		FcChar8* filename = NULL;
		FcPatternGetString(fontSet->fonts[0], FC_FILE, 0, &filename);
		return (const char*)filename;
	}
}
