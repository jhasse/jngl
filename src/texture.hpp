// Copyright 2010-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "opengl.hpp"

#include <vector>

namespace jngl {

struct Vertex;

class Texture {
public:
	Texture(float preciseWidth, float preciseHeight, int width, int height,
	        const GLubyte* const* rowPointers, // data as row pointers ...
	        GLenum format = GL_RGBA, const GLubyte* data = nullptr /* ... or as one pointer */);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;
	~Texture();
	void bind() const;
	void draw() const;
	void drawClipped(float xstart, float xend, float ystart, float yend, float red, float green,
	                 float blue, float alpha) const;
	void drawMesh(const std::vector<Vertex>& vertexes) const;
	[[nodiscard]] GLuint getID() const;
	[[nodiscard]] float getPreciseWidth() const;
	[[nodiscard]] float getPreciseHeight() const;
	void setBytes(const unsigned char*, int width, int height) const;

private:
	GLuint texture_ = 0;
	GLuint vertexBuffer_ = 0;
	GLuint vao = 0;
	std::vector<GLfloat> vertexes;
};

} // namespace jngl
