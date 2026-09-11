// Copyright 2020-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "TextLine.hpp"

#include "../FontImpl.hpp"
#include "../freetype.hpp"
#include "ScaleablePixels.hpp"
#include "font.hpp"

namespace jngl {

TextLine::TextLine(Font& font, std::string text) : text(std::move(text)), fontImpl(font.getImpl()) {
	setCenter(0, 0);
}

void TextLine::step() {
}

void TextLine::draw() const {
	//         A         <- x
	//        A A
	//       A   A
	//      A     A      <- x + lineHeight / 2 - lineSpacing / 2
	//     AAAAAAAAA
	//    A         A    <- x + lineHeight / 2
	//   A           A
	//  A             A  <- fontSize                                     ┐
	//                                                                   |
	//                                                                   ├ lineSpacing
	//                                                                   |
	//                   <- lineHeight = fontSize * LINE_HEIGHT_FACTOR   ┘

	// There's a spacing after each line using the normal setCenter would result in the
	// vertical center being off. We have to adjust by "undoing" the added space for the line.
	const double lineSpacing = fontImpl->getLineHeight() * (1 - 1 / LINE_HEIGHT_FACTOR);

	fontImpl->print(ScaleablePixels(getX()), ScaleablePixels(getY() + lineSpacing / 2.), text);
}

void TextLine::draw(Mat3 modelview) const {
	// see above
	const double lineSpacing = fontImpl->getLineHeight() * (1 - 1 / LINE_HEIGHT_FACTOR);

	fontImpl->print(modelview.translate(position + Vec2(0, lineSpacing / 2.)), text, gFontColor);
}

void TextLine::setText(std::string text) {
	this->text = std::move(text);
}

std::string TextLine::getText() const {
	return text;
}

Vec2 TextLine::getSize() const {
	return { getWidth(), getHeight() };
}

double TextLine::getWidth() const {
	return fontImpl->getTextWidth(this->text);
}

double TextLine::getHeight() const {
	return fontImpl->getLineHeight();
}

void TextLine::setCenter(double x, double y) {
	setX(x - getWidth() / 2);
	setY(y - getHeight() / 2);
}

void TextLine::setCenter(Vec2 center) {
	setX(center.x - getWidth() / 2);
	setY(center.y - getHeight() / 2);
}

double TextLine::getX() const {
	return position.x;
}

void TextLine::setX(double x) {
	position.x = x;
}

double TextLine::getY() const {
	return position.y;
}

void TextLine::setY(double y) {
	position.y = y;
}

void TextLine::setPos(double x, double y) {
	position.x = x;
	position.y = y;
}

} // namespace jngl
