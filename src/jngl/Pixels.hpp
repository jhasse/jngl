// Copyright 2021-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Pixels class
/// @file
#pragma once

#include <cstdint>

namespace jngl {

class ScaleablePixels;

/// Scale-dependent pixels, corresponds to actual pixels on the screen
class Pixels {
public:
	explicit Pixels(int32_t);
	explicit Pixels(double);

	explicit operator float() const;

	explicit operator double() const;

	explicit operator int() const;

	explicit operator ScaleablePixels() const;

	Pixels& operator+=(Pixels);

private:
	friend bool operator>(jngl::Pixels, jngl::Pixels);

	double value;
};

bool operator>(jngl::Pixels, jngl::Pixels);
Pixels operator/(Pixels, float);

} // namespace jngl

jngl::Pixels operator""_px(unsigned long long);
