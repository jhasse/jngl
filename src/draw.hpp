/*
Copyright 2009-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "opengl.hpp"

#include "jngl/screen.hpp"

#include <cmath>
#include <vector>

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
		opengl::translate(xposition, yposition);
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
	void Ellipse(const T xmid, const T ymid, T width, T height, const T startAngle) {
		width *= jngl::getScaleFactor();
		height *= jngl::getScaleFactor();
		opengl::BindArrayBuffer(0);
		glPushMatrix();
		opengl::translate(xmid, ymid);
		std::vector<T> vertexes;
		vertexes.push_back(0);
		vertexes.push_back(0);
		for (T t = startAngle; t < 2 * M_PI; t += 0.1f) {
			vertexes.push_back(width * sin(t));
			vertexes.push_back(-height * cos(t));
		}
		vertexes.push_back(0);
		vertexes.push_back(-height);
		glVertexPointer(2, opengl::Type<T>::constant, 0, &vertexes[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertexes.size() / 2);
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
