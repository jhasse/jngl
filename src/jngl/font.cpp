/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "font.hpp"

#include "../freetype.hpp"

namespace jngl {
	Font::Font() : impl(new FontImpl) {
	}

	void Font::print(const std::string& text, int x, int y) {
		impl->print(x, y, text);
	}

	boost::shared_ptr<FontImpl> Font::getImpl() {
		return impl;
	}
}