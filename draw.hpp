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

#include "opengl.hpp"

#include <cmath>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

namespace draw
{
	template<class T>
	void Rect(const T xposition, const T yposition, const T width, const T height)
	{
		opengl::BindArrayBuffer(0);
		glPushMatrix();
		opengl::Translate(xposition, yposition);
		typedef typename opengl::Type<T>::type Type;
		Type rect[] = { 0, 0,
		                static_cast<Type>(width), 0,
		                static_cast<Type>(width), static_cast<Type>(height),
		                0, static_cast<Type>(height) };
		glVertexPointer(2, opengl::Type<T>::constant, 0, rect);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glPopMatrix();
	}

	template<class T>
	void Line(const T xstart, const T ystart, const T xend, const T yend)
	{
		opengl::BindArrayBuffer(0);
		glPushMatrix();
		typedef typename opengl::Type<T>::type Type;
		Type line[] = { static_cast<Type>(xstart), static_cast<Type>(ystart),
		                static_cast<Type>(xend), static_cast<Type>(yend) };
		glVertexPointer(2, opengl::Type<T>::constant, 0, line);
		glDrawArrays(GL_LINES, 0, 2);
		glPopMatrix();
	}

	template<class T>
	void Ellipse(const T xmid, const T ymid, const T width, const T height)
	{
		opengl::BindArrayBuffer(0);
		glPushMatrix();
		opengl::Translate(xmid, ymid);
		std::vector<T> vertexes;
		int count = 0;
		for(T t = 0; t <= 2 * M_PI; t +=0.1)
		{
			vertexes.push_back(width * sin(t));
			vertexes.push_back(height * cos(t));
			++count;
		}
		glVertexPointer(2, opengl::Type<T>::constant, 0, &vertexes[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, count);
		glPopMatrix();
	}

	template<class T>
	void Point(const T x, const T y)
	{
		opengl::BindArrayBuffer(0);
		typedef typename opengl::Type<T>::type Type;
		Type point[] = { static_cast<Type>(x), static_cast<Type>(y) };
		glVertexPointer(2, opengl::Type<T>::constant, 0, point);
		glDrawArrays(GL_POINTS, 0, 1);
	}
}
