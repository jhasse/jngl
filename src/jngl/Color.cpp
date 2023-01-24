// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Color.hpp"

namespace jngl {

Color::Color(unsigned char red, unsigned char green, unsigned char blue)
: red(red), green(green), blue(blue) {
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

void Color::setRed(const unsigned char red) {
	this->red = red;
}

void Color::setGreen(const unsigned char green) {
	this->green = green;
}

void Color::setBlue(const unsigned char blue) {
	this->blue = blue;
}

} // namespace jngl

jngl::Color operator"" _rgb(const unsigned long long hex) {
	return { static_cast<unsigned char>((hex >> 16) % 256),
		     static_cast<unsigned char>((hex >> 8) % 256), static_cast<unsigned char>(hex % 256) };
}
