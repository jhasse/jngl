// Copyright 2012-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Font class and related functions
/// @file
#pragma once

#include "Rgba.hpp"
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
	///
	/// You may set \a strokePercentage to a positive or negative % value to increase or decrease
	/// the font size, without actually changing the position/size of the individual characters.
	/// This can be used to draw an outlined text by e.g. first drawing with a 5% stroke and then
	/// printing the same text with 0% (or even a negative stroke) over it. jngl::OutlinedFont does
	/// exactly that for you automatically.
	///
	/// Example: To increase the size of each character by 2px for a Font with a \a size of 20px,
	/// you would pass 10.f for \a strokePercentage.
	Font(const std::string& filename, unsigned int size, float strokePercentage = 0);

	/// Draw \a text using \a modelview in \a color
	void print(const Mat3& modelview, const std::string& text, Rgba color) const;

	/// Uses the font to print something at \a x \a y. The color can be specified using setFontColor.
	void print(const std::string&, int x, int y);

	/// Draw \a text at \a position. The color can be specified using setFontColor.
	void print(const std::string& text, Vec2 position) const;

	/// Draw \a text using \a modelview. The color can be specified using setFontColor.
	void print(const Mat3& modelview, const std::string& text) const;

	/// Calculates the width of \a text in scale-independent pixels if it would be drawn with this
	/// font
	double getTextWidth(std::string_view) const;

	/// Returns the font size in scale-independent pixels
	double getLineHeight() const;

	/// Internal function
	std::shared_ptr<FontImpl> getImpl();

private:
	std::shared_ptr<FontImpl> impl;
};

/// Print \a text at \a position
void print(const std::string& text, jngl::Vec2 position);

/// Print \a text at { \a xposition, \a yposition }
void print(const std::string& text, int xposition, int yposition);

/// Print \a text using \a modelview
void print(const Mat3& modelview, const std::string& text);

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
/// \throws std::runtime_error If the font couldn't be found.
///
/// \code
/// jngl::setFontByName("monospace");
/// \endcode
///
/// \note Not supported on iOS, Switch, Xbox, and Web. Falls back to using "Arial.ttf" on those
/// platforms.
void setFontByName(const std::string& name);

/// Sets the color and alpha value of the currently active font
void setFontColor(Rgba);

/// Sets the color of the currently active font and the alpha value
///
/// \a alpha goes from 0.0f to 1.0f (opaque). It's automatically clamped to these values.
void setFontColor(Rgb, float alpha = 1.f);

/// Sets the color of the currently active font
///
/// If the \a alpha value isn't specified, it will be set to 255 (opaque).
void setFontColor(unsigned char red, unsigned char green, unsigned char blue,
                  unsigned char alpha = 255);

/// Pushes the currently active font color on a stack and sets a new one
void pushFontColor(unsigned char red, unsigned char green, unsigned char blue);

/// Resets the font color on the top of stack which is used by jngl::pushFontColor
void popFontColor();

/// Get line height used py print() in scale-independent pixel
double getLineHeight();

/// Set line height used by print() in scale-independent pixel
void setLineHeight(double);

/// Calculates the width of \a text in scale-independent pixels if it would be drawn with the
/// currently active font
double getTextWidth(const std::string& text);

} // namespace jngl
