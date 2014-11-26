/*
Copyright 2009-2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "opengl.hpp"

namespace opengl {
	void BindArrayBuffer(const GLuint buffer) {
		static GLuint lastBuffer = 0;
		if (buffer != lastBuffer) {
			lastBuffer = buffer;
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			if (buffer) {
				glTexCoordPointer(2, GL_FLOAT, 0, 0);
				glVertexPointer(2, GL_FLOAT, 0, (void*)(8 * sizeof(GLfloat)));
			}
		}
	}
};
