/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "jngl/opengl.hpp"

namespace opengl
{
	int NextPowerOf2(const int a)
	{
		if(a == 1) // A texture with this width does not work for some reason
		{
			return 2;
		}
		int rval = 1;
		while(rval < a) rval <<= 1;
		return rval;
	}
	void BindArrayBuffer(const GLuint buffer)
	{
		static GLuint lastBuffer = 0;
		if(buffer != lastBuffer)
		{
			lastBuffer = buffer;
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			if(buffer)
			{
				glTexCoordPointer(2, GL_FLOAT, 0, 0);
				glVertexPointer(2, GL_FLOAT, 0, (void*)(8 * sizeof(GLfloat)));
			}
		}
	}
};
