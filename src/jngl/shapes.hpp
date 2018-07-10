// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"
#include "color.hpp"
#include "Vec2.hpp"

namespace jngl {

void JNGLDLL_API setColor(jngl::Color);

void JNGLDLL_API setColor(jngl::Color, unsigned char alpha);

void JNGLDLL_API setColor(unsigned char red, unsigned char green, unsigned char blue);

void JNGLDLL_API setColor(unsigned char red, unsigned char green, unsigned char blue,
                          unsigned char alpha);

void JNGLDLL_API setAlpha(unsigned char alpha);

void JNGLDLL_API pushAlpha(unsigned char alpha);

void JNGLDLL_API popAlpha();

void JNGLDLL_API setLineWidth(float width);

void JNGLDLL_API drawLine(Vec2 start, Vec2 end);

void JNGLDLL_API drawLine(double xstart, double ystart, double xend, double yend);

void JNGLDLL_API drawEllipse(float xmid, float ymid, float width, float height,
                             float startAngle = 0);

void JNGLDLL_API drawEllipse(Vec2, float width, float height, float startAngle = 0);

void JNGLDLL_API drawCircle(Vec2, float radius, float startAngle = 0);

void JNGLDLL_API drawPoint(double x, double y);

void JNGLDLL_API drawTriangle(Vec2 a, Vec2 b, Vec2 c);

void JNGLDLL_API drawTriangle(double A_x, double A_y, double B_x, double B_y, double C_x,
                              double C_y);

void JNGLDLL_API drawRect(double xposition, double yposition, double width, double height);

void JNGLDLL_API drawRect(Vec2 position, Vec2 size);

template <class Vect> void drawRect(Vect pos, Vect size) {
	drawRect(pos.x, pos.y, size.x, size.y);
}

} // namespace jngl
