/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl.hpp"
#include "time.hpp"

namespace jngl {
	double elapsedSeconds = 0;

	double getTime() {
		return elapsedSeconds;
	}

	void sleep(int) {
		// TODO
	}
}