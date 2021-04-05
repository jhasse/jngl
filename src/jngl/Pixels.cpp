// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Pixels.hpp"

namespace jngl {

Pixels::Pixels(const int32_t value) : value(value) {
}

Pixels::operator float() const {
	return value;
}

} // namespace jngl

jngl::Pixels operator "" _sp(const unsigned long long value) {
	return jngl::Pixels(value);
}
