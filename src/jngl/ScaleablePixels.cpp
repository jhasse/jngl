// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "ScaleablePixels.hpp"

namespace jngl {

ScaleablePixels::ScaleablePixels(const double value) : value(value) {
}

} // namespace jngl

jngl::ScaleablePixels operator "" _sp(const long double value) {
	return jngl::ScaleablePixels(value);
}
