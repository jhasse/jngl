/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

namespace jngl {
	class RGB {
	public:
		RGB(unsigned char red, unsigned char green, unsigned char blue);

		unsigned char getRed() const;
		unsigned char getGreen() const;
		unsigned char getBlue() const;
	private:
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	};
}