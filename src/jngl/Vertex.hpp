// Copyright 2010-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "Vec2.hpp"

namespace jngl {

/// Position and texture coordinates
struct Vertex {
	/// Screen x position
	float x;

	/// Screen y position
	float y;

	/// X coordinate in the texture, from 0 to 1
	float u;

	/// Y coordinate in the texture, from 0 to 1
	float v;
};

} // namespace jngl
