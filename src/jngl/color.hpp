// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"

namespace jngl {
	class JNGLDLL_API Color {
	public:
		Color(unsigned char red, unsigned char green, unsigned char blue);

		unsigned char getRed() const;
		void setRed(unsigned char);

		unsigned char getGreen() const;
		void setGreen(unsigned char);

		unsigned char getBlue() const;
		void setBlue(unsigned char);
	private:
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	};
}
