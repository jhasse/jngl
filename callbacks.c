/*
Copyright 2007-2008  Jan Niklas Hasse <jhasse@gmail.com>

This file is part of jngl.

jngl is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

jngl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with jngl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <GL/glu.h>
#include <stdlib.h>

GLUtesselator* tobj;

void beginCallback(GLenum which)
{
	glBegin(which);
}

void combineCallback(GLdouble coords[3],
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
	tobj = gluNewTess();
	gluTessCallback(tobj, GLU_TESS_BEGIN, beginCallback);
	gluTessCallback(tobj, GLU_TESS_VERTEX, glVertex3dv);
	gluTessCallback(tobj, GLU_TESS_END, glEnd);
	gluTessCallback(tobj, GLU_TESS_COMBINE, combineCallback);
}
