/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "color.hpp"

namespace jngl {
	Color::Color(unsigned char r, unsigned char g, unsigned char b) : red(r), green(g), blue(b) {
	}

	unsigned char Color::getRed() const {
		return red;
	}

	unsigned char Color::getGreen() const {
		return green;
	}

	unsigned char Color::getBlue() const {
		return blue;
	}
}