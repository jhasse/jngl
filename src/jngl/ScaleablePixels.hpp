// Copyright 2021-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::ScaleablePixels class
/// @file
#pragma once

namespace jngl {

class Pixels;

/// Scale-independent pixels, also called "screen pixels"
///
/// jngl::getScreenSize() will return the number of scale-independent pixels in your canvas. This is
/// also the size you can specify in jngl::AppParameters::screenSize.
class ScaleablePixels {
public:
	explicit ScaleablePixels(double);

	/// Returns the value
	explicit operator double() const;

	/// Returns the value multiplied by jngl::getScaleFactor()
	explicit operator Pixels() const;

private:
	double value;
};

} // namespace jngl

jngl::ScaleablePixels operator""_sp(long double);
jngl::ScaleablePixels operator""_sp(unsigned long long);
