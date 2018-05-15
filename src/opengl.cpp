// Copyright 2009-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "opengl.hpp"

namespace opengl {

void BindArrayBuffer(const GLuint buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	if (buffer != 0) {
		glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
		glVertexPointer(2, GL_FLOAT, 0, (void*)(8 * sizeof(GLfloat)));
	}
}

} // namespace opengl
