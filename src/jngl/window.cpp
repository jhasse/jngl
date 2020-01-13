// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../windowptr.hpp"

namespace jngl {

bool getFullscreen() {
	return pWindow->getFullscreen();
}

void setFullscreen(bool f) {
	return pWindow->setFullscreen(f);
}

} // namespace jngl
