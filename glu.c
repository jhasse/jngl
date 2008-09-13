#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <stdio.h>

GLUtesselator* tess;

void beginCallback(GLenum which)
{
	glBegin(which);
}

void tcbVertex(void *data)
{
	printf(".Vertex_1\n");
	glVertex3dv((GLdouble *)data);
	printf(".Vertex_2\n");
}

void tcbEnd()
{
	glEnd();
}

void combineCallback(GLdouble c[3], void *d[4], GLfloat w[4], void **out)
      {
	printf("ASDFASDF\n");
        GLdouble *nv = (GLdouble *) malloc(sizeof(GLdouble)*3);

        nv[0] = c[0];
        nv[1] = c[1];
        nv[2] = c[2];
        *out = nv;
	printf("Erfolgreich\n");
      }
void errorCallback(GLenum errorCode)
{
   const GLubyte *estring;
   estring = gluErrorString(errorCode);
   fprintf(stderr, "Tessellation Error: %s\n", estring);
   exit(0);
}

void InitCallbacks()
{
	tess = gluNewTess();
	gluTessCallback(tess, GLU_TESS_BEGIN, beginCallback);
	gluTessCallback(tess, GLU_TESS_VERTEX, tcbVertex);
	gluTessCallback(tess, GLU_TESS_END, tcbEnd);
	gluTessCallback(tess, GLU_TESS_ERROR, errorCallback);
	gluTessCallback(tess, GLU_TESS_COMBINE, combineCallback);
	gluTessProperty(tess, GLU_TESS_WINDING_RULE,
                   GLU_TESS_WINDING_POSITIVE);
}


void BeginPolygon()
{
	printf(".0\n");
	InitCallbacks();
	gluTessBeginPolygon(tess, NULL);
	gluTessBeginContour(tess);
	printf(".\n");
}

void Vertex(const int xposition, const int yposition)
{
	double* temp = malloc(sizeof(double)*3);
	temp[0] = (double)xposition;
	temp[1] = (double)yposition;
	temp[2] = 0.0;
	printf(".Vertex1\n");
	gluTessVertex(tess, temp, temp);
	printf(".Vertex2\n");
}

void EndPolygon()
{
	printf(".1\n");
	gluTessEndContour(tess);
	printf(".2\n");
	gluTessEndPolygon(tess);
	printf(".3\n");
}
