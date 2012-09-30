/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "text.hpp"

#include "font.hpp"
#include "../windowptr.hpp"
#include "../spriteimpl.hpp"

namespace jngl {
	Text::Text(const std::string& t) {
		font = pWindow->getFontImpl();
		setText(t);
	}

	void Text::setText(const std::string& t) {
		width = font->getTextWidth(t);
		height = font->getLineHeight();
		text = t;
	}

	void Text::setFont(boost::shared_ptr<Font> f) {
		font = f->getImpl();
		setText(text);
	}

	void Text::setAlign(Alignment a) {
		align = a;
		// TODO
	}

	void Text::step() {
	}

	void Text::draw() const {
		font->print(x, y, text);
		glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
	}
}