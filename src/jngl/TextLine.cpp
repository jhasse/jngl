// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "TextLine.hpp"

#include "../freetype.hpp"
#include "font.hpp"
#include "screen.hpp"

namespace jngl {

TextLine::TextLine(Font& font, std::string text) : text(std::move(text)), font(font.getImpl()) {
}

void TextLine::step() {
}

void TextLine::draw() const {
	font->print(x / getScaleFactor(), y / getScaleFactor(), text);
}

} // namespace jngl
