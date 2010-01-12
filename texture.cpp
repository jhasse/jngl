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

#include "texture.hpp"
#include "debug.hpp"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

bool Texture::useVBO_ = true;

Texture::Texture(const int imgWidth, const int imgHeight, GLenum format, int channels)
{
	if(useVBO_ && !(glGenBuffers && glBindBuffer && glBufferData && glDeleteBuffers))
	{
		Debug("VBOs not supported, using Vertex Arrays\n");
		useVBO_ = false;
	}
	int width = opengl::NextPowerOf2(imgWidth);
	int height = opengl::NextPowerOf2(imgHeight);
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // preventing wrapping artifacts
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	const GLfloat x = static_cast<GLfloat>(imgWidth) / static_cast<GLfloat>(width);
	const GLfloat y = static_cast<GLfloat>(imgHeight)  / static_cast<GLfloat>(height);
	if(useVBO_)
	{
		GLfloat vertexes[] = {
							   0, 0, 0, y, x, y, x, 0, // texture coordinates
							   0, 0, 0, imgHeight, imgWidth, imgHeight, imgWidth, 0
							 };
		glGenBuffers(1, &vertexBuffer_);
		opengl::BindArrayBuffer(vertexBuffer_);
		glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);
	}
	else
	{
		GLfloat texCoords[] = { 0, 0, 0, y, x, y, x, 0 };
		texCoords_.assign(&texCoords[0], &texCoords[8]);

		GLint vertexes[] = { 0, 0, 0, imgHeight, imgWidth, imgHeight, imgWidth, 0 };
		vertexes_.assign(&vertexes[0], &vertexes[8]);
	}
	for(int i = 0; i < imgHeight; ++i)
	{
		std::vector<unsigned char> empty((width - imgWidth) * channels, 0);
		glTexSubImage2D(GL_TEXTURE_2D, 0, imgWidth, i, width - imgWidth, 1, format, GL_UNSIGNED_BYTE, &empty[0]);
	}
	for(int i = imgHeight; i < height; ++i)
	{
		std::vector<unsigned char> empty(width * channels, 0);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, width, 1, format, GL_UNSIGNED_BYTE, &empty[0]);
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &texture_);
	glDeleteBuffers(1, &vertexBuffer_);
}

void Texture::Draw() const
{
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, texture_);
	if(useVBO_)
	{
		opengl::BindArrayBuffer(vertexBuffer_);
	}
	else
	{
		glVertexPointer(2, GL_INT, 0, &vertexes_[0]);
		glTexCoordPointer(2, opengl::Type<double>::constant, 0, &texCoords_[0]);
	}
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

bool Texture::GetID() const
{
	return texture_;
}
