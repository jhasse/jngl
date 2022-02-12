// Copyright 2021-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::ScaleablePixels class
/// @file
#pragma once

namespace jngl {

class Pixels;

/// Scale-independent pixels, see jngl::getScaleFactor()
class ScaleablePixels {
public:
	explicit ScaleablePixels(double);

	explicit operator double() const;

	explicit operator Pixels() const;

private:
	double value;
};

} // namespace jngl

jngl::ScaleablePixels operator"" _sp(long double);
jngl::ScaleablePixels operator"" _sp(unsigned long long);
