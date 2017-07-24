/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "dll.hpp"

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif

namespace jngl {
	void JNGLDLL_API setColor(unsigned char red, unsigned char green, unsigned char blue);

	void JNGLDLL_API setColor(unsigned char red,
	              unsigned char green,
	              unsigned char blue,
	              unsigned char alpha);

	void JNGLDLL_API setAlpha(unsigned char alpha);

	void JNGLDLL_API pushAlpha(unsigned char alpha);

	void JNGLDLL_API popAlpha();

	template<class Vect>
	void drawRect(Vect pos, Vect size) {
		DrawRect(pos.x, pos.y, size.x, size.y);
	}

	void JNGLDLL_API setLineWidth(float width);

	void JNGLDLL_API drawLine(double xstart, double ystart, double xend, double yend);

	void JNGLDLL_API drawEllipse(float xmid, float ymid, float width, float height, float startAngle = 0);

	void JNGLDLL_API drawPoint(double x, double y);
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
