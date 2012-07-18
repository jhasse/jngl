/*
Copyright 2011-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl.hpp"
#include "sdl.hpp"

namespace jngl
{
	double getTime()
	{
		SDL::init();
		Uint32 m = SDL_GetTicks();
		return static_cast<double>(m)/1000.0f;
	}
	void sleep(int milliseconds)
	{
		SDL_Delay(milliseconds);
	}
}

