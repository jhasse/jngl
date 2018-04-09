// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Vec2.hpp"

namespace jngl {

Vec2::Vec2(const double x, const double y) : x(x), y(y) {}

} // namespace jngl

jngl::Vec2 operator/(const jngl::Vec2& lhs, const double v) {
	return {lhs.x / v, lhs.y / v};
}
