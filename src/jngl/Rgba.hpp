// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
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
	constexpr Rgba(float red, float green, float blue, float alpha)
	: red(red), green(green), blue(blue), alpha(alpha) {
	}
	Rgba(float red, float green, float blue, Alpha alpha);
	Rgba(Rgb, Alpha);

	constexpr static Rgba u8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
		return Rgba{ static_cast<float>(red) / 255.f, static_cast<float>(green) / 255.f,
			         static_cast<float>(blue) / 255.f, static_cast<float>(alpha) / 255.f };
	}

	/// 0.0f ... 1.0f
	constexpr float getRed() const {
		return red;
	}
	/// 0.0f ... 1.0f
	void setRed(float);

	/// 0.0f ... 1.0fu
	constexpr float getGreen() const {
		return green;
	}
	/// 0.0f ... 1.0f
	void setGreen(float);

	/// 0.0f ... 1.0f
	constexpr float getBlue() const {
		return blue;
	}
	/// 0.0f ... 1.0f
	void setBlue(float);

	/// 0.0f ... 1.0f
	constexpr float getAlpha() const {
		return alpha;
	}
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

/// Create a jngl::Rgba object from a literal. E.g. `0x00ff00ff_rgba` for green.
jngl::Rgba operator""_rgba(unsigned long long);

namespace jngl::internal {
// Intentionally declared but not defined — see Rgb.hpp for the pattern.
void invalid_rgba_color_literal_expected_format_hash_rrggbbaa();
} // namespace jngl::internal

/// Create a jngl::Rgba object from a literal. E.g. `"#00ff00ff"_rgba` for green.
consteval jngl::Rgba operator""_rgba(const char* hex, std::size_t length) {
	using jngl::internal::hexByte;
	if (length == 9 && hex[0] == '#') {
		return jngl::Rgba::u8(hexByte(hex + 1), hexByte(hex + 3), hexByte(hex + 5),
		                      hexByte(hex + 7));
	}
	jngl::internal::invalid_rgba_color_literal_expected_format_hash_rrggbbaa();
	return jngl::Rgba{ 0, 0, 0, 0 };
}
