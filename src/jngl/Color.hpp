// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Color class
/// @file
#pragma once

#include "Rgb.hpp"

namespace jngl {

/// Object representing a RGB color
///
/// You can use the custom literal operator to create a jngl::Color from HTML color codes. For
/// example `#68da4f` becomes:
///
/// \code
/// jngl::Color color = 0x68da4f_rgb;
/// \endcode
///
/// You can use this to quickly pass a HTML code to jngl::setColor. The following lines are
/// equivalent:
///
/// \code
/// jngl::setColor(0x68da4f_rgb);
/// jngl::setColor(jngl::Color(0x68, 0xda, 0x4f));
/// jngl::setColor(jngl::Color(104, 218, 79));
/// \endcode
class Color {
public:
	/// \param red 0...255
	/// \param green 0...255
	/// \param blue 0...255
	Color(unsigned char red, unsigned char green, unsigned char blue);

	/// 0...255
	unsigned char getRed() const;
	/// 0...255
	void setRed(unsigned char);

	/// 0...255
	unsigned char getGreen() const;
	/// 0...255
	void setGreen(unsigned char);

	/// 0...255
	unsigned char getBlue() const;
	/// 0...255
	void setBlue(unsigned char);

private:
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

/// Returns a color mix between a (t == 0) and b (t == 1)
Color interpolate(Color a, Color b, float t);

/// Sets the screen's background color which is visible when nothing is drawn
void setBackgroundColor(jngl::Rgb);

/// Sets the screen's background color which is visible when nothing is drawn
/// \deprecated Use setBackgroundColor(jngl::Rgb) instead.
[[deprecated("use setBackgroundColor(jngl::Rgb) instead")]]
void setBackgroundColor(unsigned char red, unsigned char green, unsigned char blue);

} // namespace jngl
