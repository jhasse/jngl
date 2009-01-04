/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>

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

#pragma once

#ifdef OPENGLES
#include <GLES/egl.h>
#include <GLES/gl.h>
#else
#include <gl/gl.h>
#endif

namespace opengl
{
	inline void Translate(float x, float y, float z) { glTranslatef(x, y, z); }
#ifndef GL_DOUBLE
	inline void Translate(double x, double y, double z) { glTranslated(x, y, z); }
#endif
#ifdef GL_FIXED
	inline void Translate(int x, int y, int z) { glTranslatex(x, y, z); }
#endif

	template<class T> struct Type {};

	template<>
	struct Type<int>
	{
		const static GLenum constant = GL_INT;
	};

	template<>
	struct Type<float>
	{
		const static GLenum constant = GL_FLOAT;
	};

	template<>
	struct Type<double>
	{
#ifdef GL_DOUBLE
		const static GLenum constant = GL_DOUBLE;
#else
		const static GLenum constant = GL_FLOAT;
#endif
	};
}
