/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "sprite.hpp"

#include "../spriteimpl.hpp"

namespace jngl {
	Sprite::Sprite(const std::string& filename, bool halfLoad)
	: impl(new SpriteImpl(filename, halfLoad)) {
	}

	int Sprite::getWidth() const {
		return impl->Width();
	}

	int Sprite::getHeight() const {
		return impl->Height();
	}

	void Sprite::draw(Float x, Float y) const {
		impl->Draw(x, y);
	}

	void Sprite::drawScaled(Float x, Float y, float factor) const {
		impl->DrawScaled(x, y, factor, factor);
	}

	void Sprite::drawScaled(Float x, Float y,
	                        float xfactor, float yfactor) const {
		impl->DrawScaled(x, y, xfactor, yfactor);
	}

	void Sprite::drawClipped(Float x, Float y,
	                 float xstart, float xend,
	                 float ystart, float yend) const {
		impl->DrawClipped(x, y, xstart, xend, ystart, yend);
	}
}