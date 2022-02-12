// Copyright 2021-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "ScaleablePixels.hpp"

#include "Pixels.hpp"
#include "screen.hpp"

namespace jngl {

ScaleablePixels::ScaleablePixels(const double value) : value(value) {
}

ScaleablePixels::operator double() const {
	return value;
}

ScaleablePixels::operator Pixels() const {
	return Pixels(value * getScaleFactor());
}

} // namespace jngl

jngl::ScaleablePixels operator "" _sp(const long double value) {
	return jngl::ScaleablePixels(value);
}

jngl::ScaleablePixels operator "" _sp(const unsigned long long value) {
	return jngl::ScaleablePixels(static_cast<double>(value));
}
