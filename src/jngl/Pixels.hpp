// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include <cstdint>

namespace jngl {

/// Scale-dependent pixels, corresponds to actual pixels on the screen
class Pixels {
public:
	explicit Pixels(int32_t);

	explicit operator float() const;

private:
	double value;
};

} // namespace jngl

jngl::Pixels operator"" _px(long double);
