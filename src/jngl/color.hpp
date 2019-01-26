// Copyright 2012-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "dll.hpp"

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
class JNGLDLL_API Color {
public:
	Color(unsigned char red, unsigned char green, unsigned char blue);

	unsigned char getRed() const;
	void setRed(unsigned char);

	unsigned char getGreen() const;
	void setGreen(unsigned char);

	unsigned char getBlue() const;
	void setBlue(unsigned char);

private:
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};
} // namespace jngl

jngl::Color operator"" _rgb(unsigned long long);
