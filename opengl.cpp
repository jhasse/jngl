/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "opengl.hpp"

namespace opengl
{
	int NextPowerOf2(int a)
	{
		if(a == 1) // A texture with this width does not work for some reason
		{
			return 2;
		}
		int rval = 1;
		while(rval < a) rval <<= 1;
		return rval;
	}
};
