/*
Copyright 2007-2009 Jan Niklas Hasse <jhasse@gmail.com>

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

#ifndef OPENGLES

#include "sprite.hpp"
#include "opengl.hpp"

#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>

extern "C"
{
	extern GLUtesselator* tobj;
	void InitCallbacks(); // Defined in callbacks.c
}

namespace jngl
{
	std::vector<std::vector<double>*> positions;

	void BeginPolygon()
	{
		InitCallbacks();
		gluTessBeginPolygon(tobj, NULL);
		gluTessBeginContour(tobj);
	}

	void Vertex(const double xposition, const double yposition)
	{
		std::vector<double>* position = new std::vector<double>(3);
		(*position)[0] = xposition;
		(*position)[1] = yposition;
		(*position)[2] = 0;
		positions.push_back(position);
		gluTessVertex(tobj, &(*positions.back())[0], &(*positions.back())[0]);
	}

	void EndPolygon()
	{
		glColor4ub(colorRed, colorGreen, colorBlue, colorAlpha);
		gluTessEndContour(tobj);
		gluTessEndPolygon(tobj);
		std::vector<std::vector<double>*>::iterator end = positions.end();
		for(std::vector<std::vector<double>*>::iterator it = positions.begin(); it != end; ++it) {
			delete (*it);
		}
		positions.clear();
		glColor4ub(spriteColorRed, spriteColorGreen, spriteColorBlue, spriteColorAlpha);
	}
}

#else // OPENGLES

namespace jngl
{
	void BeginPolygon()
	{
	}

	void Vertex(const double xposition, const double yposition)
	{
	}

	void EndPolygon()
	{
	}
}

#endif
