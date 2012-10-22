/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "screen.hpp"
#include "types.hpp"
#include "jngl.hpp"

namespace jngl {
	Float factor = 1;

	Float getScaleFactor() {
		return factor;
	}

	void setScaleFactor(Float f) {
		factor = f;
	}

	int getScreenWidth() {
		return getWindowWidth() / factor;
	}

	int getScreenHeight() {
		return getWindowHeight() / factor;
	}
}