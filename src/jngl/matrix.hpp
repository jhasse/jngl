/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#pragma GCC visibility push(default)
namespace jngl {
	void rotate(double degree);

	void translate(double x, double y);

	template<class Vect>
	void translate(Vect v) {
		translate(v.x, v.y);
	}

	void scale(double factor);

	void scale(double xfactor, double yfactor);

	void pushMatrix();

	void popMatrix();

	void reset();
}
#pragma GCC visibility pop