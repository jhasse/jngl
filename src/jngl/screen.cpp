/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "screen.hpp"
#include "types.hpp"
#include "jngl.hpp"

namespace jngl {
	Float factor;

	int getScreenWidth() {
		return getWindowWidth();
	}

	int getScreenHeight() {
		return getWindowHeight();
	}
}