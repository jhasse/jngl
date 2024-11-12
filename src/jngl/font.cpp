// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "font.hpp"

#include "../freetype.hpp"
#include "Alpha.hpp"
#include "ScaleablePixels.hpp"

#include <stack>

namespace jngl {

Rgba gFontColor{ 0, 0, 0, 1 };

void setFontColor(const Rgba color) {
	gFontColor = color;
}

void setFontColor(const Rgb color, float alpha) {
	gFontColor = Rgba(color, Alpha(alpha));
}

void setFontColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
	gFontColor = Rgba::u8(red, green, blue, alpha);
}

std::stack<Rgb> rgbs;

void pushFontColor(unsigned char red, unsigned char green, unsigned char blue) {
	rgbs.push(static_cast<Rgb>(gFontColor));
	setFontColor(red, green, blue);
}

void popFontColor() {
	gFontColor.setRgb(rgbs.top());
	rgbs.pop();
}

Font::Font(const std::string& filename, unsigned int size, float strokePercentage)
: impl(new FontImpl(filename, size, strokePercentage)) {
}

void Font::print(const Mat3& modelview, const std::string& text, Rgba color) const {
	impl->print(modelview, text, color);
}

void Font::print(const std::string& text, int x, int y) {
	impl->print(ScaleablePixels(x), ScaleablePixels(y), text);
}

void Font::print(const std::string& text, const Vec2 position) const {
	impl->print(ScaleablePixels(position.x), ScaleablePixels(position.y), text);
}

void Font::print(const Mat3& modelview, const std::string& text) const {
	impl->print(modelview, text, gFontColor);
}

double Font::getTextWidth(std::string_view text) {
	return static_cast<double>(static_cast<ScaleablePixels>(impl->getTextWidth(std::string(text))));
}

std::shared_ptr<FontImpl> Font::getImpl() {
	return impl;
}

} // namespace jngl
