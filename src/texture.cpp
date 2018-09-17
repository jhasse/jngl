// Copyright 2010-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "texture.hpp"

#include <boost/math/special_functions/relative_difference.hpp>
#include <cassert>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace jngl {

std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

Texture::Texture(const int width, const int height, const GLubyte* const* const rowPointers,
                 GLenum format, const GLubyte* const data)
: width(width), height(height) {
#ifdef EPOXY_PUBLIC
	static bool first = true;
	if (first && !epoxy_has_gl_extension("GL_ARB_vertex_buffer_object")) {
		throw std::runtime_error("VBOs not supported\n");
	}
	first = false;
#endif
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
	                GL_CLAMP_TO_EDGE); // preventing wrapping artifacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLfloat vertexes[] = { 0,
		                   0,
		                   0,
		                   1,
		                   1,
		                   1,
		                   1,
		                   0, // texture coordinates
		                   0,
		                   0,
		                   0,
		                   GLfloat(height),
		                   GLfloat(width),
		                   GLfloat(height),
		                   GLfloat(width),
		                   0 };
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glGenBuffers(1, &vertexBuffer_);
	opengl::BindArrayBuffer(vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);
	texCoords_.assign(&vertexes[0], &vertexes[8]);
	vertexes_.assign(&vertexes[8], &vertexes[16]);

	if (rowPointers) {
		assert(!data);
		for (int i = 0; i < height; ++i) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, width, 1, format, GL_UNSIGNED_BYTE,
			                rowPointers[i]);
		}
	}
	if (data) {
		assert(!rowPointers);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
	}

	glBindVertexArray(0);
}

Texture::~Texture() {
	glDeleteTextures(1, &texture_);
	glDeleteBuffers(1, &vertexBuffer_);
	glDeleteVertexArrays(1, &vao);
}

void Texture::draw() const {
#ifdef ANDROID
	// Android only supports VAOs with OpenGL ES 2.0, which JNGL isn't using yet.
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	opengl::BindArrayBuffer(vertexBuffer_);
#else
	glBindVertexArray(vao);
#endif
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture_);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisable(GL_TEXTURE_2D);
#ifdef ANDROID
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#else
	glBindVertexArray(0);
#endif
}

void Texture::drawClipped(const float xstart, const float xend, const float ystart,
                          const float yend) const {
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindTexture(GL_TEXTURE_2D, texture_);
	opengl::BindArrayBuffer(0);
	auto tmpVertexes = vertexes_;
	assert(boost::math::epsilon_difference(tmpVertexes[0], 0) < 9 &&
	       boost::math::epsilon_difference(tmpVertexes[1], 0) < 9 &&
	       boost::math::epsilon_difference(tmpVertexes[2], 0) < 9 &&
	       boost::math::epsilon_difference(tmpVertexes[7], 0) < 9);
	tmpVertexes[4] *= (xend - xstart);
	tmpVertexes[6] *= (xend - xstart);
	tmpVertexes[3] *= (yend - ystart);
	tmpVertexes[5] *= (yend - ystart);
	auto tmpTexCoords = texCoords_;
	tmpTexCoords[0] = tmpTexCoords[2] = (tmpTexCoords[4] * xstart);
	tmpTexCoords[1] = tmpTexCoords[7] = (tmpTexCoords[3] * ystart);
	tmpTexCoords[4] *= xend;
	tmpTexCoords[6] *= xend;
	tmpTexCoords[3] *= yend;
	tmpTexCoords[5] *= yend;
	glVertexPointer(2, GL_FLOAT, 0, &tmpVertexes[0]);
	glTexCoordPointer(2, opengl::Type<double>::constant, 0, &tmpTexCoords[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

GLuint Texture::getID() const {
	return texture_;
}

int Texture::getWidth() const {
	return width;
}

int Texture::getHeight() const {
	return height;
}

} // namespace jngl
