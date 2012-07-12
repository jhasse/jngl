/*
Copyright 2007-2011 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#if defined (OPENGLES) || defined (__APPLE__)

#include "jngl.hpp"

namespace jngl
{
	void beginPolygon()
	{
	}
    
	void vertex(const double xposition, const double yposition)
	{
	}
    
	void endPolygon()
	{
	}
}

#else

#include "sprite.hpp"
#include "opengl.hpp"
#include "callbacks.h"

#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>

extern "C" {
extern GLUtesselator* tobj;
}

namespace jngl
{
	std::vector<std::vector<double>*> positions;

	void beginPolygon()
	{
		InitCallbacks();
		gluTessBeginPolygon(tobj, NULL);
		gluTessBeginContour(tobj);
	}

	void vertex(const double xposition, const double yposition)
	{
		std::vector<double>* position = new std::vector<double>(3);
		position->at(0) = xposition;
		position->at(1) = yposition;
		position->at(2) = 0;
		positions.push_back(position);
		gluTessVertex(tobj, &position->at(0), &position->at(0));
	}

	void endPolygon()
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

#endif
