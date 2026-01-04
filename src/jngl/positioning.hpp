// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Useful functions to position elements on the screen
/// @file
#pragma once

#include "Vec2.hpp"

namespace jngl {

/// Pass an object T that has methods setCenter(jngl::Vec2) and getWidth(). This function will
/// position the object so that the right edge is at \a position.
template <class T> void setRightEdge(T& obj, jngl::Vec2 position) {
	obj.setCenter(jngl::Vec2(position.x - obj.getWidth() / 2., position.y));
}

} // namespace jngl
