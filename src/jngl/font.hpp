// Copyright 2012-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Font class and related functions
/// @file
#pragma once

#include "Color.hpp"
#include "Vec2.hpp"

#include <memory>
#include <string>

namespace jngl {

class FontImpl;
class Mat3;

/// Font loaded from a TTF or OTF file
class Font {
public:
	/// Creates a font from \a filename in \a size px
	Font(const std::string& filename, unsigned int size);

	/// Uses the font to print something at \a x \a y. The color can be specified using setFontColor.
	void print(const std::string&, int x, int y);

	/// Draw \a text at \a position
	void print(const std::string& text, Vec2 position) const;

	/// Draw \a text using \a modelview
	void print(const Mat3& modelview, const std::string& text) const;

	/// Internal function
	std::shared_ptr<FontImpl> getImpl();

private:
	std::shared_ptr<FontImpl> impl;
};

/// Print \a text at \a position
void print(const std::string& text, jngl::Vec2 position);

/// Print \a text at { \a xposition, \a yposition }
void print(const std::string& text, int xposition, int yposition);

/// Get the font size used by print()
int getFontSize();

/// Change the font size used by print()
void setFontSize(int size);

/// Returns the currently active font name
///
/// This will either return a font name if jngl::setFont was used or a font name if
/// jngl::setFontByName was used.
std::string getFont();

/// Sets the currently active font to \a filename
///
/// This can either be a TTF or OTF file.
void setFont(const std::string& filename);

/// Sets the currently active font by a font \a name
///
/// \code
/// jngl::setFontByName("monospace");
/// \endcode
void setFontByName(const std::string& name);

/// Sets the color of the currently active font
void setFontColor(jngl::Color);

/// Sets the color of the currently active font and the alpha value
///
/// \a alpha goes from 0.0f to 1.0f (opaque). It's automatically clamped to these values.
void setFontColor(Color, float alpha);

/// Sets the color of the currently active font
///
/// If the \a alpha value isn't specified, it will be set to 255 (opaque).
void setFontColor(unsigned char red, unsigned char green, unsigned char blue,
                  unsigned char alpha = 255);

/// Pushes the currently active font color on a stack and sets a new one
void pushFontColor(unsigned char red, unsigned char green, unsigned char blue);

/// Resets the font color on the top of stack which is used by jngl::pushFontColor
void popFontColor();

/// Get line height used py print() in pixel
int getLineHeight();

/// Set line height used by print() in pixel
void setLineHeight(int);

/// Calculates the width of \a text in pixels if it would be drawn with the currently active font
double getTextWidth(const std::string& text);

} // namespace jngl
