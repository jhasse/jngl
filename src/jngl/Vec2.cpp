// Copyright 2018-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Vec2.hpp"

#include <cmath>
#include <iostream>

namespace jngl {

Vec2::Vec2() = default;

Vec2::Vec2(const double x, const double y) : x(x), y(y) {
}

bool Vec2::isNull() const {
	return std::fpclassify(x) == FP_ZERO && std::fpclassify(y) == FP_ZERO;
}

void Vec2::rotate(float angle) {
	boost::qvm::mat<float, 2, 2> rotationMatrix = { std::cos(angle), -std::sin(angle),
		                                            std::sin(angle), std::cos(angle) };
	*this = rotationMatrix * *this;
}

std::ostream& operator<<(std::ostream& os, const Vec2& v) {
	return os << "[x=" << v.x << ", y=" << v.y << "]";
}

} // namespace jngl
