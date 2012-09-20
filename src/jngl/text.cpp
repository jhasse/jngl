/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "text.hpp"
#include "jngl.hpp"

namespace jngl {
	Text::Text(const std::string& t) {
		setText(t);
	}

	void Text::setText(const std::string& t) {
		width = getTextWidth(t);
		height = getLineHeight();
		text = t;
	}

	void Text::step() {
	}
	
	void Text::draw() const {
		print(text, x, y);
	}
}