/*
Copyright 2009-2010 Jan Niklas Hasse <jhasse@gmail.com>

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

#ifdef WIZ
	#include <GLES/gl.h>
	#include <GLES/egl.h>
	#include <libogl.h>
#else
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glext.h>
	#ifndef linux
		#include <windows.h>
		#define GL_ARRAY_BUFFER 0x8892
		#define GL_STATIC_DRAW 0x88E4
		typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
		typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
		typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
		typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);
		extern PFNGLGENBUFFERSARBPROC glGenBuffers;
		extern PFNGLBINDBUFFERARBPROC glBindBuffer;
		extern PFNGLBUFFERDATAARBPROC glBufferData;
		extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;
	#endif
#endif

#include <boost/function.hpp>

#ifndef GL_BGR
#define GL_BGR 0x80e0
#endif

namespace opengl
{
	inline void Translate(float x, float y) { glTranslatef(x, y, 0); }
	inline void Scale(float x, float y) { glScalef(x, y, 0); }
#ifdef GL_DOUBLE
	template<class T, class U>
	inline void Translate(T x, U y) { glTranslated(static_cast<double>(x), static_cast<double>(y), 0); }
	template<class T, class U>
	inline void Scale(T x, U y) { glScaled(static_cast<double>(x), static_cast<double>(y), 0); }
#else
	template<class T, class U>
	inline void Translate(T x, U y) { Translate(static_cast<float>(x), static_cast<float>(y)); }
	template<class T, class U>
	inline void Scale(T x, U y) { Scale(static_cast<float>(x), static_cast<float>(y)); }
#endif

	template<class T> struct Type {};

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
		typedef GLdouble type;
#else
		const static GLenum constant = GL_FLOAT;
		typedef GLfloat type;
#endif
	};

#ifdef GL_DOUBLE
	typedef GLdouble CoordType;
#else
	typedef GLfloat CoordType;
#endif

	// This function gets the first power of 2 >= the int that we pass it.
	int NextPowerOf2(int);

	void BindArrayBuffer(GLuint);
}
