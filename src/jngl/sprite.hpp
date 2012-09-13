/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "types.hpp"

#include <memory>
#include <boost/noncopyable.hpp>
#include <string>

namespace jngl {
	class SpriteImpl;

	class Sprite : boost::noncopyable {
	public:
		Sprite(const std::string& filename, bool halfLoad);
		int getWidth() const;
		int getHeight() const;
		void draw(Float x, Float y) const;
		void drawScaled(Float x, Float y, float factor) const;
		void drawScaled(Float x, Float y, float xfactor, float yfactor) const;
		void drawClipped(Float x, Float y,
		                 float xstart, float xend,
		                 float ystart, float yend) const;
	private:
		std::auto_ptr<SpriteImpl> impl;
	};

	void setSpriteColor(unsigned char red,
	                    unsigned char green,
	                    unsigned char blue,
	                    unsigned char alpha);

	void setSpriteColor(unsigned char red,
	                    unsigned char green,
	                    unsigned char alpha);

	void setSpriteAlpha(unsigned char alpha);
}