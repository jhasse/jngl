/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "rgb.hpp"

namespace jngl {
	RGB::RGB(unsigned char r, unsigned char g, unsigned char b) : red(r), green(g), blue(b) {
	}

	unsigned char RGB::getRed() const {
		return red;
	}

	unsigned char RGB::getGreen() const {
		return green;
	}

	unsigned char RGB::getBlue() const {
		return blue;
	}
}