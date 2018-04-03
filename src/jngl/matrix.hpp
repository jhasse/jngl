// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"

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
