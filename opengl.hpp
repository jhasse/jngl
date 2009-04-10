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
#include <GL/gl.h>
#endif

#include <boost/function.hpp>

namespace opengl
{
	inline void Translate(float x, float y) { glTranslatef(x, y, 0); }
#ifdef GL_DOUBLE
	template<class T, class U>
	inline void Translate(T x, U y) { glTranslated(static_cast<double>(x), static_cast<double>(y), 0); }
#else
	template<class T, class U>
	inline void Translate(T x, U y) { Translate(static_cast<float>(x), static_cast<float>(y)); }
#endif
#ifdef GL_FIXED
	inline void Translate(int x, int y) { glTranslatex(x, y, 0); }

	#ifndef GL_INT
		#define GL_INT GL_FIXED
	#endif
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

	// Display lists don't exist in OpenGL ES so we need a wrapper
	class DisplayList {
	public:
#ifndef OPENGLES
		DisplayList();
		~DisplayList();
#endif
		void Create(boost::function<void()> function);
		void Call() const;
	private:
#ifdef OPENGLES
		boost::function<void()> function_;
#else
		GLuint displayList_;
#endif
	};

	// This function gets the first power of 2 >= the int that we pass it.
	int NextPowerOf2(int);
}
