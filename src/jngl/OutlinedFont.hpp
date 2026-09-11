// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::OutlinedFont class
/// @file
#pragma once

#include "font.hpp"

namespace jngl {

class FontInterface;

/// Same as jngl::Font but adds an outline
class OutlinedFont {
public:
	/// Set \a strokePercentage to a positive value in %, e.g. 5 to add an outline of 5% to the font
	OutlinedFont(const std::string& filename, unsigned int size, float strokePercentage);

	/// Print \a text using \a modelview in \a innerColor and an outline in \a outerColor
	void print(const Mat3& modelview, const std::string& text, jngl::Rgba innerColor,
	           jngl::Rgba outerColor) const;

	/// Print \a text using \a modelview in \a innerColor and an outline in \a outerColor
	void printCentered(Mat3 modelview, const std::string& text, jngl::Rgba innerColor,
	                   jngl::Rgba outerColor) const;

	/// Calculates the width of \a text in scale-independent pixels if it would be drawn with this
	/// font
	double getTextWidth(std::string_view) const;

	/// Creates a new FontInterface with pre-baked colors for the inner and outer (outline) text
	/// @param innerColor The color to use for the main text
	/// @param outerColor The color to use for the outline
	/// @return A shared pointer to a FontInterface that can be used for rendering text with the
	/// specified colors
	///
	/// \note The OutlinedFont can be safely destroyed while the returned FontInterface is still in
	/// use.
	///
	/// Example:
	/// @code
	/// jngl::Text text("Outlined Text");
	/// text.setFont(jngl::OutlinedFont("Arial.ttf", 20, 5.f)
	///                 .bake(jngl::Rgba(1, 1, 1, 1), jngl::Rgba(0, 0, 0, 1)));
	/// @endcode
	std::shared_ptr<FontInterface> bake(jngl::Rgba innerColor, jngl::Rgba outerColor) const;

private:
	jngl::Font inner;
	jngl::Font outer;
};

} // namespace jngl
