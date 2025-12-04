// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Rect class
/// @file
#pragma once

#include "Mat3.hpp"
#include "Vec2.hpp"

namespace jngl {

/// Simple struct for a rectangle, can be use with jngl::contains
struct Rect {
	double getX() const;
	double getY() const;
	double getWidth() const;
	double getHeight() const;

	/// Draws a red box around the rectangle
	void drawBoundingBox(Mat3 modelview) const;

	Vec2 pos;
	Vec2 size;
};

} // namespace jngl
