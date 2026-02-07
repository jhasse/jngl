// Copyright 2009-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/message.hpp"

#include "unicode.hpp"

#include <string>
#include <iostream>
#include <windows.h>

namespace jngl {

void printMessage(const std::string& text) {
	std::cout << text << std::flush;
}

} // namespace jngl
