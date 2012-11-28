/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "font.hpp"

#include "../freetype.hpp"
#include "color.hpp"
#include "screen.hpp"

#include <stack>

namespace jngl {
	unsigned char fontColorRed = 0, fontColorGreen = 0, fontColorBlue = 0, fontColorAlpha = 255;

	void setFontColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
		fontColorRed = red;
		fontColorGreen = green;
		fontColorBlue = blue;
		fontColorAlpha = alpha;
	}

	std::stack<Color> rgbs;

	void pushFontColor(unsigned char red, unsigned char green, unsigned char blue) {
		rgbs.push(Color(fontColorRed, fontColorGreen, fontColorBlue));
		setFontColor(red, green, blue);
	}

	void popFontColor() {
		fontColorRed = rgbs.top().getRed();
		fontColorGreen = rgbs.top().getGreen();
		fontColorBlue = rgbs.top().getBlue();
		rgbs.pop();
	}

	Font::Font(const std::string& filename, unsigned int size)
	: impl(new FontImpl(filename.c_str(), size)) {
	}

	void Font::print(const std::string& text, int x, int y) {
		impl->print(x, y, text);
	}

	boost::shared_ptr<FontImpl> Font::getImpl() {
		return impl;
	}
}