// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "OutlinedFont.hpp"

#include "Mat3.hpp"

namespace jngl {

OutlinedFont::OutlinedFont(const std::string& filename, unsigned int size, float strokePercentage)
: inner(filename, size), outer(filename, size, strokePercentage) {
}

void OutlinedFont::print(const Mat3& modelview, const std::string& text, Rgba innerColor,
                         Rgba outerColor) const {
	outer.print(modelview, text, outerColor);
	inner.print(modelview, text, innerColor);
}

void OutlinedFont::printCentered(Mat3 modelview, const std::string& text, Rgba innerColor,
                                 Rgba outerColor) const {
	modelview.translate({ -getTextWidth(text) / 2, 0 });
	outer.print(modelview, text, outerColor);
	inner.print(modelview, text, innerColor);
}

double OutlinedFont::getTextWidth(std::string_view text) const {
	return inner.getTextWidth(text);
}

} // namespace jngl
