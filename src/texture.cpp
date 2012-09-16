/*
Copyright 2010-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "texture.hpp"
#include "jngl/debug.hpp"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace jngl {
	bool Texture::useVBO_ = true;

	Texture::Texture(const int imgWidth, const int imgHeight, GLubyte** rowPointers, GLenum format, int channels, GLubyte* data) {
		if (useVBO_ && (!GLEW_ARB_vertex_buffer_object || !GLEW_VERSION_1_5)) {
			jngl::debug("VBOs not supported, using Vertex Arrays\n");
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
		if (useVBO_) {
			GLfloat vertexes[] = {
								   0, 0, 0, y, x, y, x, 0, // texture coordinates
								   0, 0, 0, GLfloat(imgHeight), GLfloat(imgWidth), GLfloat(imgHeight), GLfloat(imgWidth), 0
								 };
			glGenBuffers(1, &vertexBuffer_);
			opengl::BindArrayBuffer(vertexBuffer_);
			glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);
		}

		GLfloat texCoords[] = { 0, 0, 0, y, x, y, x, 0 };
		texCoords_.assign(&texCoords[0], &texCoords[8]);
		GLfloat vertexes[] = { 0, 0, 0, float(imgHeight), float(imgWidth), float(imgHeight), float(imgWidth), 0 };
		vertexes_.assign(&vertexes[0], &vertexes[8]);

		if (rowPointers) {
			for (int i = 0; i < imgHeight; ++i) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, imgWidth, 1, format, GL_UNSIGNED_BYTE, rowPointers[i]);
			}

			// Add one addional pixel line to the edges
			if (imgWidth < width) {
				for (int i = 0; i < imgHeight; ++i) {
					glTexSubImage2D(GL_TEXTURE_2D, 0, imgWidth, i, 1, 1, format, GL_UNSIGNED_BYTE, &rowPointers[i][(imgWidth-1)*channels]);
				}
			}
			if (imgHeight < height) {
				std::vector<char> lastLine(rowPointers[imgHeight-1], rowPointers[imgHeight-1]+imgWidth*channels);
				lastLine.push_back(lastLine.back());
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, imgHeight, imgWidth+1, 1, format, GL_UNSIGNED_BYTE, &lastLine[0]);
			}
		}
		if (data) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imgWidth, imgHeight, format, GL_UNSIGNED_BYTE, data);

			// Add one addional pixel line to the edges
			if (imgWidth < width) {
				for (int i = 0; i < imgHeight; ++i) {
					glTexSubImage2D(GL_TEXTURE_2D, 0, imgWidth, i, 1, 1, format, GL_UNSIGNED_BYTE,
					                data + ((i + 1) * imgWidth  - 1) * channels);
				}
			}
			if (imgHeight < height) {
				std::vector<char> lastLine(data + (imgHeight-1) * imgWidth * channels,
				                           data +  imgHeight    * imgWidth * channels);
				lastLine.push_back(lastLine.back());
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, imgHeight, imgWidth+1, 1, format, GL_UNSIGNED_BYTE, &lastLine[0]);
			}
		}
	}

	Texture::~Texture() {
		glDeleteTextures(1, &texture_);
		if (useVBO_) {
			glDeleteBuffers(1, &vertexBuffer_);
		}
	}

	void Texture::draw() const {
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindTexture(GL_TEXTURE_2D, texture_);
		if (useVBO_) {
			opengl::BindArrayBuffer(vertexBuffer_);
		}
	#ifndef WIZ
		else {
			glVertexPointer(2, GL_FLOAT, 0, &vertexes_[0]);
			glTexCoordPointer(2, opengl::Type<double>::constant, 0, &texCoords_[0]);
		}
	#endif
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
}