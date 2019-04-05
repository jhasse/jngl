// Copyright 2012-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "types.hpp"

#include "dll.hpp"

namespace jngl {
class JNGLDLL_API Drawable {
public:
	Drawable();
	Drawable(const Drawable&) = default;
	virtual ~Drawable();
	virtual void step() = 0;
	virtual void draw() const = 0;

	virtual void setPos(Float x, Float y);

	template <class Vect> Vect getCenter() const {
		return Vect(getX() + getWidth() / 2, getY() + getHeight() / 2);
	}

	virtual void setCenter(Float x, Float y);

	template <class Vect> void setCenter(Vect c) {
		setCenter(c.x, c.y);
	}

	/// Returns the distance from the left side of the screen
	Float getLeft() const;
	void setLeft(Float x);

	/// Returns the distance from the top of the screen
	Float getTop() const;
	void setTop(Float y);

	/// Returns the distance from the right side of the screen
	Float getRight() const;
	void setRight(Float x);

	/// Returns the distance from the bottom of the screen.
	Float getBottom() const;
	void setBottom(Float y);

	Float getX() const;
	void setX(Float);

	Float getY() const;
	void setY(Float);

	/// Returns the width in screen coordinates
	float getWidth() const;

	/// Returns the height in screen coordinates
	float getHeight() const;

protected:
	Float x;
	Float y;
	float width;
	float height;
};
} // namespace jngl
