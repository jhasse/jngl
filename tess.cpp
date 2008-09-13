#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <stdio.h>

#include <vector>

extern GLUtesselator* tobj;

extern "C"
{
	void InitCallbacks(); // Defined in callbacks.c
}

namespace jngl
{
	std::vector<std::vector<double> > positions;
	
	void BeginPolygon()
	{
		InitCallbacks();
		gluTessBeginPolygon(tobj, NULL);
		gluTessBeginContour(tobj);
	}

	void Vertex(const double xposition, const double yposition)
	{
		std::vector<double> position(3);
		position[0] = xposition;
		position[1] = yposition;
		position[2] = 0;
		positions.push_back(position);
		gluTessVertex(tobj, &positions.back()[0], &positions.back()[0]);
	}

	void EndPolygon()
	{
		gluTessEndContour(tobj);
		gluTessEndPolygon(tobj);
		positions.clear(); // free all stored positions
	}}
