// Copyright 2010-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "texture.hpp"

#include "jngl/Mat3.hpp"
#include "jngl/Rgba.hpp"
#include "jngl/Vertex.hpp"
#include "jngl/screen.hpp"

#include <cassert>

#ifdef JNGL_VULKAN
#include "Renderer.hpp"
#include "log.hpp"
#include "vulkan/VulkanRenderer.hpp"
#else
#include "ShaderCache.hpp"
#endif

namespace jngl {

#ifdef JNGL_VULKAN
namespace {
VulkanRenderer& vulkanRenderer() {
	return static_cast<VulkanRenderer&>(getRenderer());
}
} // namespace
#endif

Texture::Texture(const float preciseWidth, const float preciseHeight, const int width,
                 const int height, const GLubyte* const* const rowPointers, GLenum format,
                 const GLubyte* const data, const GLenum type)
#ifndef JNGL_VULKAN
: texture_(opengl::genAndBindTexture())
#endif
{
#ifndef JNGL_VULKAN
	assert(format == GL_RGB || format == GL_RGBA || format == GL_BGR);
	GLint internalFormat = format == GL_RGBA ? GL_RGBA : GL_RGB;
	if (type == GL_HALF_FLOAT) {
		internalFormat = GL_RGBA16F;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
#endif
	vertexes = {
		0, 0,
		0, 0, // texture coordinates
		0, static_cast<float>(preciseHeight / getScaleFactor()),
		0, 1, // texture coordinates
		static_cast<float>(preciseWidth / getScaleFactor()), static_cast<float>(preciseHeight / getScaleFactor()),
		1, 1, // texture coordinates
		static_cast<float>(preciseWidth / getScaleFactor()), 0,
		1, 0 // texture coordinates
	};
#ifdef JNGL_VULKAN
	vkTexture = vulkanRenderer().createTexture(width, height, format, type, data, rowPointers);
#else
	glGenVertexArrays(1, &vao);
	opengl::bindVertexArray(vao);

	glGenBuffers(1, &vertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertexes.data(), GL_STATIC_DRAW);

	const GLint posAttrib =
	    ShaderCache::handle().textureShaderProgram->getAttribLocation("position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	const GLint texCoordAttrib =
	    ShaderCache::handle().textureShaderProgram->getAttribLocation("inTexCoord");
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
	                      reinterpret_cast<void*>(2 * sizeof(float))); // NOLINT
	glEnableVertexAttribArray(texCoordAttrib);

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
#endif
}

Texture::~Texture() {
#ifdef JNGL_VULKAN
	if (vkTexture) {
		if (auto* renderer = getRendererIfExists()) {
			static_cast<VulkanRenderer*>(renderer)->destroyTexture(*vkTexture);
		}
	}
#else
	if (ShaderCache::handleIfAlive()) {
		// if the ShaderCache doesn't exist anymore, this means unloadAll() has been called by
		// hideWindow() and the OpenGL context doesn't exist anymore. It's unnecessary to delete
		// OpenGL resources in that case. It might even lead to crashes when the OpenGL function
		// pointers have been unloaded (Windows).
		glDeleteTextures(1, &texture_);
		glDeleteBuffers(1, &vertexBuffer_);
		opengl::deleteVertexArray(vao);
	}
#endif
}

void Texture::bind() const {
#ifndef JNGL_VULKAN
	opengl::bindVertexArray(vao);

	glBindTexture(GL_TEXTURE_2D, texture_);
#endif
}

void Texture::draw() const {
#ifndef JNGL_VULKAN
	bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
#endif
}

void Texture::draw(const Mat3& modelview, const Rgba color) const {
#ifdef JNGL_VULKAN
	if (vkTexture) {
		vulkanRenderer().drawSprite(*vkTexture, vertexes.data(), 4, PrimitiveType::TriangleFan,
		                            modelview, color);
	}
#else
	opengl::bindVertexArray(vao);
	auto& shaderCache = ShaderCache::handle();
	auto context = shaderCache.textureShaderProgram->use();
	glUniform4f(shaderCache.shaderSpriteColorUniform, color.getRed(), color.getGreen(),
	            color.getBlue(), color.getAlpha());
	glUniformMatrix3fv(shaderCache.modelviewUniform, 1, GL_FALSE, modelview.data);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
#endif
}

void Texture::drawClipped(const float xstart, const float xend, const float ystart,
                          const float yend, const float red, const float green, const float blue,
                          const float alpha) const {
	std::vector<float> vertexes = this->vertexes;

	vertexes[8] *= (xend - xstart);
	vertexes[12] *= (xend - xstart);
	vertexes[5] *= (yend - ystart);
	vertexes[9] *= (yend - ystart);

	// Texture coordinates:
	vertexes[2] = vertexes[6] = xstart;
	vertexes[3] = vertexes[15] = ystart;
	vertexes[10] = vertexes[14] = xend;
	vertexes[7] = vertexes[11] = yend;

#ifdef JNGL_VULKAN
	if (vkTexture) {
		vulkanRenderer().drawSprite(*vkTexture, vertexes.data(), 4, PrimitiveType::TriangleFan,
		                            opengl::modelview, Rgba(red, green, blue, alpha));
	}
#else
	opengl::bindVertexArray(opengl::vaoStream);
	auto& shaderCache = ShaderCache::handle();
	auto tmp = shaderCache.textureShaderProgram->use();
	glUniform4f(shaderCache.shaderSpriteColorUniform, red, green, blue, alpha);
	glUniformMatrix3fv(shaderCache.modelviewUniform, 1, GL_FALSE, opengl::modelview.data);
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexes.size() * sizeof(float)),
	             vertexes.data(), GL_STREAM_DRAW);

	const GLint posAttrib = shaderCache.textureShaderProgram->getAttribLocation("position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	const GLint texCoordAttrib = shaderCache.textureShaderProgram->getAttribLocation("inTexCoord");
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
	                      reinterpret_cast<void*>(2 * sizeof(float))); // NOLINT
	glEnableVertexAttribArray(texCoordAttrib);

	glBindTexture(GL_TEXTURE_2D, texture_);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
#endif
}

void Texture::drawMesh(const std::vector<Vertex>& vertexes) const {
#ifdef JNGL_VULKAN
	// drawMesh relies on the caller having set the modelview/color as ambient shader state, which
	// the Vulkan backend doesn't have. Supporting it needs the transform and color threaded through
	// (see Sprite::drawMesh); not done yet.
	(void)vertexes;
	static bool warned = false;
	if (!warned) {
		internal::warn("jngl::Sprite::drawMesh is not yet implemented on the Vulkan backend.");
		warned = true;
	}
#else
	opengl::bindVertexArray(opengl::vaoStream);
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexes.size() * sizeof(vertexes[0])),
	             vertexes.data(), GL_STREAM_DRAW);

	auto& shaderCache = ShaderCache::handle();
	const GLint posAttrib = shaderCache.textureShaderProgram->getAttribLocation("position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
	glEnableVertexAttribArray(posAttrib);

	const GLint texCoordAttrib = shaderCache.textureShaderProgram->getAttribLocation("inTexCoord");
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
	                      reinterpret_cast<void*>(2 * sizeof(float))); // NOLINT
	glEnableVertexAttribArray(texCoordAttrib);

	glBindTexture(GL_TEXTURE_2D, texture_);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexes.size()));
#endif
}

GLuint Texture::getID() const {
#ifdef JNGL_VULKAN
	return 0;
#else
	return texture_;
#endif
}

float Texture::getPreciseWidth() const {
	return vertexes[8] * getScaleFactor();
}

float Texture::getPreciseHeight() const {
	return vertexes[5] * getScaleFactor();
}

void Texture::setBytes(const unsigned char* const bytes, const int width, const int height) const {
#ifdef JNGL_VULKAN
	if (vkTexture) {
		vulkanRenderer().updateTextureBytes(*vkTexture, width, height, bytes);
	}
#else
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
#endif
}

} // namespace jngl
