// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
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
	constexpr Rgb(float red, float green, float blue) : red(red), green(green), blue(blue) {
	}

	constexpr static Rgb u8(uint8_t red, uint8_t green, uint8_t blue) {
		return Rgb{ static_cast<float>(red) / 255.f, static_cast<float>(green) / 255.f,
			        static_cast<float>(blue) / 255.f };
	}

	/// Implicit conversion for backwards compatibility
	Rgb(Color); // NOLINT

	/// 0.0f ... 1.0f
	constexpr float getRed() const {
		return red;
	}
	/// 0.0f ... 1.0f
	void setRed(float);

	/// 0.0f ... 1.0f
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

namespace jngl::internal {
// These are intentionally declared but not defined. When reached in a consteval context,
// the compiler error will include the function name as the diagnostic message.
void invalid_hex_character_in_color_literal();
void invalid_rgb_color_literal_expected_format_hash_rrggbb();

consteval uint8_t hexVal(char c) {
	if (c >= '0' && c <= '9') {
		return static_cast<uint8_t>(c - '0');
	}
	if (c >= 'a' && c <= 'f') {
		return static_cast<uint8_t>(c - 'a' + 10);
	}
	if (c >= 'A' && c <= 'F') {
		return static_cast<uint8_t>(c - 'A' + 10);
	}
	invalid_hex_character_in_color_literal();
	return 0;
}
consteval uint8_t hexByte(const char* s) {
	return static_cast<uint8_t>(hexVal(s[0]) * 16 + hexVal(s[1]));
}
} // namespace jngl::internal

/// Create a jngl::Rgb object from a literal. E.g. `"#00ff00"_rgb` for green.
consteval jngl::Rgb operator""_rgb(const char* hex, std::size_t length) {
	using jngl::internal::hexByte;
	if (length == 7 && hex[0] == '#') {
		return jngl::Rgb::u8(hexByte(hex + 1), hexByte(hex + 3), hexByte(hex + 5));
	}
	jngl::internal::invalid_rgb_color_literal_expected_format_hash_rrggbb();
	return jngl::Rgb{ 0, 0, 0 };
}
