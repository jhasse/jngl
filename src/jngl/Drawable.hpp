// Copyright 2012-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Drawable class
/// @file
#pragma once

#include "Vec2.hpp"

namespace jngl {

/// Base class for drawable objects with a position and a rectangle size
class Drawable {
public:
	Drawable();
	Drawable(const Drawable&) = default;
	Drawable& operator=(const Drawable&) = default;
	Drawable(Drawable&&) = default;
	Drawable& operator=(Drawable&&) = default;
	virtual ~Drawable();

	/// Advance object's state
	virtual void step() = 0;

	/// Called when drawing a frame
	virtual void draw() const = 0;

	/// Sets the position of the top-left of the Drawable
	virtual void setPos(double x, double y);

	/// Sets the position of the top-left of the Drawable
	template <class Vect> void setPos(Vect p) {
		setPos(p.x, p.y);
	}

	/// Returns the position of the center of the Drawable
	jngl::Vec2 getCenter() const;

	virtual void setCenter(double x, double y);

	template <class Vect> void setCenter(Vect c) {
		setCenter(c.x, c.y);
	}

	/// Returns the distance from the left side of the screen
	double getLeft() const;
	void setLeft(double x);

	/// Returns the distance from the top of the screen
	double getTop() const;
	void setTop(double y);

	/// Returns the distance from the right side of the screen
	double getRight() const;
	void setRight(double x);

	/// Returns the distance from the bottom of the screen.
	double getBottom() const;
	void setBottom(double y);

	double getX() const;
	void setX(double);

	double getY() const;
	void setY(double);

	/// Returns {width, height} in screen coordinates
	Vec2 getSize() const;

	/// Returns the width in screen coordinates
	float getWidth() const;

	/// Returns the height in screen coordinates
	float getHeight() const;

	void drawBoundingBox() const;

	/// Returns whether \a point is inside the bounding box
	bool contains(jngl::Vec2 point) const;

protected:
	Vec2 position;
	float width = 0;
	float height = 0;
};

template <class Box> bool contains(const Box& box, const Vec2 point) {
	return (box.getX() <= point.x && point.x < box.getX() + box.getWidth() &&
	        box.getY() <= point.y && point.y < box.getY() + box.getHeight());
}

} // namespace jngl
