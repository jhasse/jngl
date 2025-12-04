// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Rect.hpp"

#include "Rgba.hpp"
#include "shapes.hpp"

namespace jngl {

double Rect::getX() const {
	return pos.x;
}

double Rect::getY() const {
	return pos.y;
}

double Rect::getWidth() const {
	return size.x;
}

double Rect::getHeight() const {
	return size.y;
}

void Rect::drawBoundingBox(Mat3 modelview) const {
	jngl::drawRect(modelview.translate(pos), size, 0xff0000aa_rgba);
}

} // namespace jngl
