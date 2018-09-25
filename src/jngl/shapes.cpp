// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "shapes.hpp"

#include "../spriteimpl.hpp"

namespace jngl {

uint8_t colorRed = 0, colorGreen = 0, colorBlue = 0, colorAlpha = 255;

void setColor(const jngl::Color color) {
	setColor(color.getRed(), color.getGreen(), color.getBlue());
}

void setColor(const jngl::Color color, const unsigned char alpha) {
	setColor(color.getRed(), color.getGreen(), color.getBlue(), alpha);
}

void setColor(unsigned char red, unsigned char green, unsigned char blue) {
	colorRed = red;
	colorGreen = green;
	colorBlue = blue;
}

void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	colorRed = red;
	colorGreen = green;
	colorBlue = blue;
	colorAlpha = alpha;
}

void setAlpha(unsigned char alpha) {
	colorAlpha = alpha;
}

std::stack<unsigned char> alphas;

void pushAlpha(unsigned char alpha) {
	alphas.push(colorAlpha);
	setAlpha(colorAlpha * alpha / 255);
}

void popAlpha() {
	setAlpha(alphas.top());
	alphas.pop();
}

void drawEllipse(float xmid, float ymid, float width, float height, float startAngle) {
	pWindow->drawEllipse({xmid, ymid}, {width, height}, startAngle);
}

void drawEllipse(const Vec2 position, const float width, const float height,
                 const float startAngle) {
	pWindow->drawEllipse(position, {width, height}, startAngle);
}

void drawCircle(const Vec2 position, const float radius, const float startAngle) {
	pWindow->drawEllipse(position, {radius, radius}, startAngle);
}

} // namespace jngl
