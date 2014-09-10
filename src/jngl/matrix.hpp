/*
Copyright 2012-2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "dll.hpp"

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif

namespace jngl {
	void JNGLDLL_API rotate(double degree);

	void JNGLDLL_API translate(double x, double y);

	template<class Vect>
	void translate(Vect v) {
		translate(v.x, v.y);
	}

	void JNGLDLL_API scale(double factor);

	void JNGLDLL_API scale(double xfactor, double yfactor);

	void JNGLDLL_API pushMatrix();

	void JNGLDLL_API popMatrix();

	void JNGLDLL_API reset();
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
