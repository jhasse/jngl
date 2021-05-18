// Copyright 2020-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "TextLine.hpp"

#include "../freetype.hpp"
#include "ScaleablePixels.hpp"
#include "font.hpp"
#include "screen.hpp"

namespace jngl {

TextLine::TextLine(Font& font, std::string text) : text(std::move(text)), fontImpl(font.getImpl()) {
	width = static_cast<float>(fontImpl->getTextWidth(this->text));
	height = static_cast<float>(fontImpl->getLineHeight());
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
	//                   <- lineHeight = fontSize * LINE_HEIGHT_FACOTR   ┘

	// There's a spacing after each line using the normal setCenter would result in the
	// vertical center being off. We have to adjust by "undoing" the added space for the line.
	const double lineSpacing = fontImpl->getLineHeight() * (1 - 1 / LINE_HEIGHT_FACOTR);

	fontImpl->print(getX(), getY() + lineSpacing / 2. / getScaleFactor(), text);
}

} // namespace jngl
