/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "types.hpp"

#pragma GCC visibility push(default)
namespace jngl {
	class Drawable {
	public:
		Drawable();
		virtual ~Drawable();
		virtual void step() = 0;
		virtual void draw() const = 0;

		virtual void setPos(Float x, Float y);

		template<class Vect>
		Vect getCenter() const {
			return Vect{x + width/2, y + height/2};
		}

		virtual void setCenter(Float x, Float y);

		template<class Vect>
		void setCenter(Vect c) {
			setCenter(c.x, c.y);
		}

		Float getLeft() const;
		void setLeft(Float x);

		Float getTop() const;
		void setTop(Float y);

		Float getRight() const;
		void setRight(Float x);

		Float getBottom() const;
		void setBottom(Float y);

		Float getX() const;
		void setX(Float);

		Float getY() const;
		void setY(Float);

		int getWidth() const;
		int getHeight() const;
	protected:
		Float x;
		Float y;
		int width;
		int height;
	};
}
#pragma GCC visibility pop
