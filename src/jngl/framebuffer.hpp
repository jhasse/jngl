// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains FrameBuffer class
/// @file
#pragma once

#include "Color.hpp"
#include "Finally.hpp"
#include "Vec2.hpp"
#include "Vertex.hpp"

#include <memory>

namespace jngl {

class ShaderProgram;

/// Image framebuffer object which can be rendered on
class FrameBuffer {
public:
	/// Creates a framebuffer object with \a width times \a height pixels
	FrameBuffer(int width, int height);

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = default;
	FrameBuffer& operator=(FrameBuffer&&) = default;
	~FrameBuffer();

	/// Lifetime object when the FrameBuffer is in use
	struct Context {
		explicit Context(std::function<void()> resetCallback);
		~Context();

		void clear(Color);

	private:
		std::function<void()> resetCallback;
	};

#if __cplusplus >= 201703L
	[[nodiscard]]
#endif
	Context use() const;

	/// Draws the framebuffer image to the screen
	void draw(Vec2 position, const ShaderProgram* = nullptr) const;
	void draw(double x, double y) const;

	/// Draws a list of triangles with the framebuffer's texture on it
	void drawMesh(const std::vector<Vertex>& vertexes, const ShaderProgram* = nullptr) const;

	static void clear();

	/// Returns the size in screen pixels
	Vec2 getSize() const;

	/// Returns the OpenGL texture ID of the associated image buffer
	///
	/// While this is an implementation detail, it can be useful if you want to draw the
	/// `GL_TEXTURE_2D` object yourself or need to pass it to another library. The return type is
	/// equivalent to `GLuint`.
	uint32_t getTextureID() const;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
