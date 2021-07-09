// Copyright 2020-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"
#include "../jngl/Controller.hpp"

#include <Foundation/Foundation.h>

namespace jngl {

std::string getBinaryPath() {
	return getPrefix(); // better than nothing
}

std::string getPreferredLanguage() {
	return [[[NSLocale preferredLanguages] firstObject] UTF8String];
}

} // namespace jngl
