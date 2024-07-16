// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Rgb.hpp"

namespace jngl {

Rgb::Rgb(float red, float green, float blue) : red(red), green(green), blue(blue) {
}

Rgb Rgb::u8(uint8_t red, uint8_t green, uint8_t blue) {
	return Rgb{ static_cast<float>(red) / 255.f, static_cast<float>(green) / 255.f,
		        static_cast<float>(blue) / 255.f };
}

float Rgb::getRed() const {
	return red;
}

float Rgb::getGreen() const {
	return green;
}

float Rgb::getBlue() const {
	return blue;
}

void Rgb::setRed(const float red) {
	this->red = red;
}

void Rgb::setGreen(const float green) {
	this->green = green;
}

void Rgb::setBlue(const float blue) {
	this->blue = blue;
}

Rgb interpolate(Rgb a, Rgb b, float t) {
	return { a.getRed() * (1.f - t) + b.getRed() * t, a.getGreen() * (1.f - t) + b.getGreen() * t,
		     a.getBlue() * (1.f - t) + b.getBlue() * t };
}

} // namespace jngl
