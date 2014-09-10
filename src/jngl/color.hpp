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
	class JNGLDLL_API Color {
	public:
		Color(unsigned char red, unsigned char green, unsigned char blue);

		unsigned char getRed() const;
		unsigned char getGreen() const;
		unsigned char getBlue() const;
	private:
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	};
}

#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
