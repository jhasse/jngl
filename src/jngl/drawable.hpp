// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "types.hpp"

#include "dll.hpp"

namespace jngl {
	class JNGLDLL_API Drawable {
	public:
		Drawable();
		virtual ~Drawable();
		virtual void step() = 0;
		virtual void draw() const = 0;

		virtual void setPos(Float x, Float y);

		template<class Vect>
		Vect getCenter() const {
			return Vect(getX() + getWidth()/2, getY() + getHeight()/2);
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

		float getWidth() const;
		float getHeight() const;

	protected:
		Float x;
		Float y;
		float width;
		float height;
	};
}
