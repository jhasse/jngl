/*
Copyright 2010-2015 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "texture.hpp"
#include "jngl/debug.hpp"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace jngl {
	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

	Texture::Texture(const int width, const int height, GLubyte** rowPointers,
	                 GLenum format, int channels, GLubyte* data) : width(width), height(height) {
		if (!GLEW_ARB_vertex_buffer_object || !GLEW_VERSION_1_5) {
			throw std::runtime_error("VBOs not supported\n");
		}
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // preventing wrapping artifacts
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		GLfloat vertexes[] = {
			0, 0, 0, 1, 1, 1, 1, 0, // texture coordinates
			0, 0, 0, GLfloat(height), GLfloat(width), GLfloat(height), GLfloat(width), 0
		};
		glGenBuffers(1, &vertexBuffer_);
		opengl::BindArrayBuffer(vertexBuffer_);
		glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);
		texCoords_.assign(&vertexes[0], &vertexes[8]);
		vertexes_.assign(&vertexes[8], &vertexes[16]);

		if (rowPointers) {
			for (int i = 0; i < height; ++i) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, width, 1, format, GL_UNSIGNED_BYTE, rowPointers[i]);
			}
		}
		if (data) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
		}
	}

	Texture::~Texture() {
		glDeleteTextures(1, &texture_);
		glDeleteBuffers(1, &vertexBuffer_);
	}

	void Texture::draw() const {
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindTexture(GL_TEXTURE_2D, texture_);
		opengl::BindArrayBuffer(vertexBuffer_);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	void Texture::drawClipped(const float xstart, const float xend, const float ystart, const float yend) const {
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindTexture(GL_TEXTURE_2D, texture_);
		opengl::BindArrayBuffer(0);
		auto tmpVertexes = vertexes_;
		tmpVertexes[0] = tmpVertexes[2] = (tmpVertexes[4] * xstart);
		tmpVertexes[1] = tmpVertexes[7] = (tmpVertexes[3] * ystart);
		tmpVertexes[4] *= xend;
		tmpVertexes[6] *= xend;
		tmpVertexes[3] *= yend;
		tmpVertexes[5] *= yend;
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
}
