// Copyright 2021-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Pixels.hpp"

#include "ScaleablePixels.hpp"
#include "screen.hpp"

#include <cmath>
#include <gsl/narrow>

namespace jngl {

Pixels::Pixels(const int32_t value) : value(value) {
}

Pixels::Pixels(const double value) : value(value) {
}

Pixels::operator float() const {
	return static_cast<float>(value);
}

Pixels::operator double() const {
	return value;
}

Pixels::operator int() const {
	return int(std::lround(value));
}

Pixels::operator ScaleablePixels() const {
	return ScaleablePixels(value / getScaleFactor());
}

Pixels& Pixels::operator+=(const Pixels b) {
	value += b.value;
	return *this;
}

bool operator>(const jngl::Pixels a, const jngl::Pixels b) {
	return a.value > b.value;
}

Pixels operator/(const jngl::Pixels a, const float b) {
	return Pixels(float(a) / b);
}

} // namespace jngl

jngl::Pixels operator "" _px(const unsigned long long value) {
	return jngl::Pixels(gsl::narrow<int32_t>(value));
}
