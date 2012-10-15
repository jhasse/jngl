/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#pragma GCC visibility push(default)
namespace jngl {
	void setColor(unsigned char red, unsigned char green, unsigned char blue);

	void setColor(unsigned char red,
	              unsigned char green,
	              unsigned char blue,
	              unsigned char alpha);

	void setAlpha(unsigned char alpha);

	void pushAlpha(unsigned char alpha);

	void popAlpha();

	template<class Vect>
	void drawRect(Vect pos, Vect size) {
		DrawRect(pos.x, pos.y, size.x, size.y);
	}

	void drawLine(double xstart, double ystart, double xend, double yend);

	void drawEllipse(float xmid, float ymid, float width, float height, float startAngle = 0);

	void drawPoint(double x, double y);
}
#pragma GCC visibility pop