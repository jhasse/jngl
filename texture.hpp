/*
Copyright 2010 Jan Niklas Hasse <jhasse@gmail.com>

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

#include "opengl.hpp"

#include <vector>
#include <boost/noncopyable.hpp>

class Texture : boost::noncopyable {
public:
	Texture(int imgWidth, int imgHeight, GLenum format = GL_RGBA, int channels = 4);
	~Texture();
	void Bind() const;
	void Draw() const;
	void DrawClipped(float xstart, float xend, float ystart, float yend) const;
	GLuint GetID() const;
private:
	static bool useVBO_;
	GLuint texture_;
	GLuint vertexBuffer_;
	std::vector<opengl::CoordType> texCoords_;
	std::vector<GLint> vertexes_;
};
