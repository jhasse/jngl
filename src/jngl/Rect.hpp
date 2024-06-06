// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Rect class
/// @file
#pragma once

#include "Vec2.hpp"

namespace jngl {

/// Simple struct for a rectangle, can be use with jngl::contains
struct Rect {
    double getX() const { return pos.x; }
    double getY() const { return pos.y; }
    double getWidth() const { return size.x; }
    double getHeight() const { return size.y; }

	Vec2 pos;
    Vec2 size;
};

} // namespace jngl
