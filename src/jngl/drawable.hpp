/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "types.hpp"

namespace jngl {
	class Drawable {
	public:
		Drawable();
		virtual ~Drawable();
		virtual void step() = 0;
		virtual void draw() const = 0;

		void setPos(Float x, Float y);

		Float getLeft() const;
		void setLeft(Float x);

		Float getTop() const;
		void setTop(Float y);

		Float getRight() const;
		void setRight(Float x);

		Float getBottom() const;
		void setBottom(Float y);

		int getWidth() const;
		int getHeight() const;
	protected:
		Float x;
		Float y;
		Float width;
		Float height;
	};
}