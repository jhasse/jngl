// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Rgba class
/// @file
#pragma once

#include "Rgb.hpp"

#include <cstdint>

namespace jngl {
class Alpha;

/// Object representing a RGBA color
///
/// You can use the custom literal operator to create jngl::Rgba from HTML color codes. For example
/// `#68da4fee` becomes:
///
/// \code
/// jngl::Rgba color = 0x68da4fee_rgba;
/// \endcode
class Rgba {
public:
	/// \param red 0.0f ... 1.0f
	/// \param green 0.0f ... 1.0f
	/// \param blue 0.0f ... 1.0f
	/// \param alpha 0.0f ... 1.0f
	///
	/// Values over 1.0f will result in 1.0f and negatives will result in 0.0f.
	Rgba(float red, float green, float blue, float alpha);
	Rgba(float red, float green, float blue, Alpha alpha);
	Rgba(Rgb, Alpha);

	static Rgba u8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

	/// 0.0f ... 1.0f
	float getRed() const;
	/// 0.0f ... 1.0f
	void setRed(float);

	/// 0.0f ... 1.0fu
	float getGreen() const;
	/// 0.0f ... 1.0f
	void setGreen(float);

	/// 0.0f ... 1.0f
	float getBlue() const;
	/// 0.0f ... 1.0f
	void setBlue(float);

	/// 0.0f ... 1.0f
	float getAlpha() const;
	/// 0.0f ... 1.0f
	void setAlpha(float);
	void setAlpha(Alpha);

	/// Overwrites red, green and blue, but leaves the alpha value untouched
	void setRgb(Rgb color);

	/// Drops the alpha value
	explicit operator Rgb() const;

private:
	float red;
	float green;
	float blue;
	float alpha;
};

/// Returns a color mix between a (t == 0) and b (t == 1)
Rgba interpolate(Rgba a, Rgba b, float t);

} // namespace jngl

/// Create a jngl::Rgba object from a literal. E.g. `0x00ff00_rgb` for green.
jngl::Rgba operator""_rgba(unsigned long long);
