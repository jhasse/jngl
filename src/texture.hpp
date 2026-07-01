// Copyright 2010-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "opengl.hpp"

#include <memory>
#include <vector>

namespace jngl {

struct Vertex;
class Mat3;
class Rgba;
struct VulkanTexture;

class Texture {
public:
	Texture(float preciseWidth, float preciseHeight, int width, int height,
	        const GLubyte* const* rowPointers, // data as row pointers ...
	        GLenum format = GL_RGBA, const GLubyte* data = nullptr /* ... or as one pointer */,
	        // Channel type of the texture's storage. Pass GL_HALF_FLOAT for an HDR texture with
	        // floating point channels (used by the high precision FrameBuffer); the default stores
	        // each channel as an 8-bit integer mapped to the range [0, 1].
	        GLenum type = GL_UNSIGNED_BYTE);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator=(Texture&&) = delete;
	~Texture();
	void bind() const;
	void draw() const;

	/// Draws the texture's quad transformed by \a modelview and modulated by \a color, using the
	/// built-in texture shader. This is the backend-neutral path (required by the Vulkan backend,
	/// which can't rely on ambient shader state like the OpenGL backend does).
	void draw(const Mat3& modelview, Rgba color) const;
	void drawClipped(float xstart, float xend, float ystart, float yend, float red, float green,
	                 float blue, float alpha) const;
	void drawMesh(const std::vector<Vertex>& vertexes) const;

	/// Backend-neutral mesh draw: transforms \a vertexes by \a modelview, modulated by \a color
	/// (and the active ShaderProgram, if any). Needed by the Vulkan backend, which can't rely on
	/// ambient shader state.
	void drawMesh(const std::vector<Vertex>& vertexes, const Mat3& modelview, Rgba color) const;
	[[nodiscard]] GLuint getID() const;
	[[nodiscard]] float getPreciseWidth() const;
	[[nodiscard]] float getPreciseHeight() const;
	void setBytes(const unsigned char*, int width, int height) const;

private:
#ifdef JNGL_VULKAN
	std::unique_ptr<VulkanTexture> vkTexture;
#else
	GLuint texture_ = 0;
	GLuint vertexBuffer_ = 0;
	GLuint vao = 0;
#endif
	std::vector<GLfloat> vertexes;
};

} // namespace jngl
