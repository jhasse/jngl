// Copyright 2009-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "opengl.hpp"

#include "jngl/matrix.hpp"
#include "jngl/screen.hpp"
#include "main.hpp"

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
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		jngl::pushMatrix();
		opengl::translate(xposition * jngl::getScaleFactor(), yposition * jngl::getScaleFactor());
		auto _ = jngl::useSimpleShaderProgram();
		typedef typename opengl::Type<T>::type Type;
		Type rect[] = { 0, 0,
		                static_cast<Type>(width * jngl::getScaleFactor()), 0,
						static_cast<Type>(width * jngl::getScaleFactor()),
						static_cast<Type>(height * jngl::getScaleFactor()),
		                0, static_cast<Type>(height * jngl::getScaleFactor()) };
		glVertexPointer(2, opengl::Type<T>::constant, 0, rect);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		jngl::popMatrix();
	}

	template<class T>
	void Line(const T xstart, const T ystart, const T xend, const T yend) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		jngl::pushMatrix();
		auto _ = jngl::useSimpleShaderProgram();
		typedef typename opengl::Type<T>::type Type;
		Type line[] = { static_cast<Type>(xstart * jngl::getScaleFactor()),
		                static_cast<Type>(ystart * jngl::getScaleFactor()),
		                static_cast<Type>(xend * jngl::getScaleFactor()),
		                static_cast<Type>(yend * jngl::getScaleFactor()) };
		glVertexPointer(2, opengl::Type<T>::constant, 0, line);
		glDrawArrays(GL_LINES, 0, 2);
		jngl::popMatrix();
	}

	template<class T>
	void Triangle(const T A_x, const T A_y, const T B_x, const T B_y, const T C_x, const T C_y) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		auto _ = jngl::useSimpleShaderProgram();
		typedef typename opengl::Type<T>::type Type;
		Type line[] = { static_cast<Type>(A_x * jngl::getScaleFactor()),
		                static_cast<Type>(A_y * jngl::getScaleFactor()),
		                static_cast<Type>(B_x * jngl::getScaleFactor()),
		                static_cast<Type>(B_y * jngl::getScaleFactor()),
		                static_cast<Type>(C_x * jngl::getScaleFactor()),
		                static_cast<Type>(C_y * jngl::getScaleFactor()) };
		glVertexPointer(2, opengl::Type<T>::constant, 0, line);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	template<class T>
	void Ellipse(const T xmid, const T ymid, const T width, const T height, const T startAngle) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		jngl::pushMatrix();
		glScalef(jngl::getScaleFactor(), jngl::getScaleFactor(), 1);
		opengl::translate(xmid, ymid);
		auto _ = jngl::useSimpleShaderProgram();
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
		glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertexes.size() / 2));
		jngl::popMatrix();
	}

	template<class T>
	void Point(const T x, const T y)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		auto _ = jngl::useSimpleShaderProgram();
		typedef typename opengl::Type<T>::type Type;
		Type point[] = { static_cast<Type>(x), static_cast<Type>(y) };
		glVertexPointer(2, opengl::Type<T>::constant, 0, point);
		glDrawArrays(GL_POINTS, 0, 1);
	}
}
