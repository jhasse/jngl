// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Rgba.hpp"

#include "Alpha.hpp"

namespace jngl {

Rgba::Rgba(float red, float green, float blue, float alpha)
: red(red), green(green), blue(blue), alpha(alpha) {
}

Rgba::Rgba(const Rgb color, const Alpha alpha)
: red(color.getRed()), green(color.getGreen()), blue(color.getBlue()), alpha(alpha.getAlpha()) {
}

Rgba Rgba::u8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
	return Rgba{ static_cast<float>(red) / 255.f, static_cast<float>(green) / 255.f,
		         static_cast<float>(blue) / 255.f, static_cast<float>(alpha) / 255.f };
}

float Rgba::getRed() const {
	return red;
}

float Rgba::getGreen() const {
	return green;
}

float Rgba::getBlue() const {
	return blue;
}

float Rgba::getAlpha() const {
	return alpha;
}

void Rgba::setRed(const float red) {
	this->red = red;
}

void Rgba::setGreen(const float green) {
	this->green = green;
}

void Rgba::setBlue(const float blue) {
	this->blue = blue;
}

void Rgba::setAlpha(const float alpha) {
	this->alpha = alpha;
}

void Rgba::setAlpha(const Alpha alpha) {
	this->alpha = alpha.getAlpha();
}

void Rgba::setRgb(Rgb color) {
	red = color.getRed();
	green = color.getGreen();
	blue = color.getBlue();
}

Rgba::operator Rgb() const {
	return { red, green, blue };
}

Rgba interpolate(Rgba a, Rgba b, float t) {
	return { a.getRed() * (1.f - t) + b.getRed() * t, a.getGreen() * (1.f - t) + b.getGreen() * t,
		     a.getBlue() * (1.f - t) + b.getBlue() * t,
		     a.getAlpha() * (1.f - t) + b.getAlpha() * t };
}

} // namespace jngl

jngl::Rgba operator"" _rgba(const unsigned long long hex) {
	return jngl::Rgba::u8((hex >> 24) % 256, (hex >> 16) % 256, (hex >> 8) % 256, hex % 256);
}
