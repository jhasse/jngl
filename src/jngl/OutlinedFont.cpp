// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "OutlinedFont.hpp"

namespace jngl {

OutlinedFont::OutlinedFont(const std::string& filename, unsigned int size, float strokePercentage)
: inner(filename, size, -strokePercentage / 2.), outer(filename, size, strokePercentage / 2.) {
}

void OutlinedFont::print(const Mat3& modelview, const std::string& text, Rgba innerColor,
                         Rgba outerColor) const {
	outer.print(modelview, text, outerColor);
	inner.print(modelview, text, innerColor);
}

double OutlinedFont::getTextWidth(std::string_view text) {
	return inner.getTextWidth(text);
}

} // namespace jngl
