/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "jngl.hpp"

namespace jngl {
	void Work::onQuitEvent() {
		jngl::quit();
	}

	void Work::onLoad() {
	}

	Work::~Work() {
	}
}
