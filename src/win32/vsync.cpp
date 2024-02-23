// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/other.hpp"

#include <glad/wgl.h>

namespace jngl {

bool vsyncEnabled = false;

void setVerticalSync(bool enabled) {
	if (wglSwapIntervalEXT && wglSwapIntervalEXT(enabled ? 1 : 0)) {
		vsyncEnabled = enabled;
	}
}

bool getVerticalSync() {
	return vsyncEnabled;
}

} // namespace jngl
