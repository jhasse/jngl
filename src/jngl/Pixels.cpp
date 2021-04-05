// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Pixels.hpp"

#include "ScaleablePixels.hpp"
#include "screen.hpp"

#include <boost/numeric/conversion/cast.hpp>

namespace jngl {

Pixels::Pixels(const int32_t value) : value(value) {
}

Pixels::Pixels(const double value) : value(value) {
}

Pixels::operator float() const {
	return value;
}

Pixels::operator ScaleablePixels() const {
	return ScaleablePixels(value * getScaleFactor());
}

Pixels& Pixels::operator+=(const Pixels b) {
	value += b.value;
	return *this;
}

bool operator>(const jngl::Pixels a, const jngl::Pixels b) {
	return a.value > b.value;
}

} // namespace jngl

jngl::Pixels operator "" _px(const unsigned long long value) {
	return jngl::Pixels(boost::numeric_cast<int32_t>(value));
}
