// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../windowptr.hpp"

namespace jngl {

bool JNGLDLL_API getFullscreen() {
	return pWindow->getFullscreen();
}

void JNGLDLL_API setFullscreen(bool f) {
	return pWindow->setFullscreen(f);
}

} // namespace jngl
