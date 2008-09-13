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
