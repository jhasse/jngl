/*
Copyright 2007-2011 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "callbacks.h"

#if defined (OPENGLES) || defined (__APPLE__)

void InitCallbacks() {}

#else

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glu.h>
#include <stdlib.h>

GLUtesselator* tobj = NULL;

#ifdef _WIN32
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif

void CALLBACK beginCallback(GLenum which)
{
	glBegin(which);
}

void CALLBACK combineCallback(GLdouble coords[3],
	                 GLdouble *vertex_data[4],
	                 GLfloat weight[4], GLdouble **dataOut )
{
   GLdouble *vertex;
   int i;

   vertex = (GLdouble *) malloc(6 * sizeof(GLdouble));

   vertex[0] = coords[0];
   vertex[1] = coords[1];
   vertex[2] = coords[2];
   for (i = 3; i < 7; i++)
	  vertex[i] = weight[0] * vertex_data[0][i]
	              + weight[1] * vertex_data[1][i]
	              + weight[2] * vertex_data[2][i]
	              + weight[3] * vertex_data[3][i];
   *dataOut = vertex;
}

void InitCallbacks()
{
	if(tobj == NULL) {
		tobj = gluNewTess();
		gluTessCallback(tobj, GLU_TESS_BEGIN, beginCallback);
		gluTessCallback(tobj, GLU_TESS_VERTEX, glVertex3dv);
		gluTessCallback(tobj, GLU_TESS_END, glEnd);
		gluTessCallback(tobj, GLU_TESS_COMBINE, combineCallback);
	}
}

#endif