// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Rgb class
/// @file
#pragma once

#include <cstdint>
#include <iosfwd>

namespace jngl {

class Color;

/// Object representing a RGB color, new version of jngl::Color (which will be deprecated in the
/// future)
class Rgb {
public:
	/// \param red 0.0f ... 1.0f
	/// \param green 0.0f ... 1.0f
	/// \param blue 0.0f ... 1.0f
	///
	/// Values over 1.0f will result in 1.0f and negatives will result in 0.0f.
	Rgb(float red, float green, float blue);

	static Rgb u8(uint8_t red, uint8_t green, uint8_t blue);

	/// Implicit conversion for backwards compatibility
	Rgb(Color); // NOLINT

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

	/// 0 ... 255
	uint8_t getRed_u8() const;

	/// 0 ... 255
	uint8_t getGreen_u8() const;

	/// 0 ... 255
	uint8_t getBlue_u8() const;

	/// Conversion for backwards compatibility
	explicit operator Color() const;

private:
	float red;
	float green;
	float blue;
};

/// Returns a color mix between a (t == 0) and b (t == 1)
Rgb interpolate(Rgb a, Rgb b, float t);

/// Returns the screen's background color which is visible when nothing is drawn and also used by
/// jngl::Fade
Rgb getBackgroundColor();

/// Returns true if the two colors are equal in 24 bit SDR space, i.e. each channel rounded to
/// uint8_t
bool operator==(Rgb a, Rgb b);

/// Prints the color as `jngl::Rgb{ red, green, blue }`
std::ostream& operator<<(std::ostream& os, Rgb color);

} // namespace jngl

/// Create a jngl::Rgb object from a literal. E.g. `0x00ff00_rgb` for green.
jngl::Rgb operator""_rgb(unsigned long long);
