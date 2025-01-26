// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::OutlinedFont class
/// @file
#pragma once

#include "font.hpp"

namespace jngl {

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

private:
	jngl::Font inner;
	jngl::Font outer;
};

} // namespace jngl
