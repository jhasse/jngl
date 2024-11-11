// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::FrameBuffer class
/// @file
#pragma once

#include "Color.hpp"
#include "Finally.hpp"
#include "Mat3.hpp"
#include "Pixels.hpp"
#include "Vec2.hpp"
#include "Vertex.hpp"

#include <memory>
#include <vector>

namespace jngl {

class ShaderProgram;

/// Image framebuffer object which can be rendered on
///
/// Example:
/// \code
/// FrameBuffer foo(300_px, 200_px);
/// // ...
/// {
///     auto context = foo.use();
///     context.clear(0xff0000_rgb);
///     jngl::print("Hello World!", jngl::Vec2(-100, -20));
/// } // jngl::FrameBuffer::Context is destroyed, now drawing on the main buffer again
/// // ...
/// foo.draw(jngl::modelview()); // draws a red rectangle with Hello World! on it
/// \endcode
class FrameBuffer {
public:
	/// Creates a framebuffer object with \a width times \a height actual pixels
	FrameBuffer(Pixels width, Pixels height);

	/// Creates a framebuffer object with \a width times \a height scalable pixels
	FrameBuffer(ScaleablePixels width, ScaleablePixels height);

	/// Creates a framebuffer object with \a size[0] times \a size[1] pixels
	explicit FrameBuffer(std::array<Pixels, 2> size);

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = default;
	FrameBuffer& operator=(FrameBuffer&&) = default;
	~FrameBuffer();

	/// Lifetime object when the FrameBuffer is in use
	struct Context {
		explicit Context(std::function<void()> resetCallback);
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) noexcept;
		Context& operator=(Context&&) noexcept;
		~Context();

		/// Clear the framebuffer with a transparent background
		void clear();

		/// Clear the framebuffer with \a color
		void clear(Rgb color);

	private:
		std::function<void()> resetCallback;
	};

	/// Starts drawing on the FrameBuffer as long as Context is alive
	///
	/// The modelview matrix gets adjusted so that (0, 0) is in the center of the FrameBuffer.
#if __cplusplus >= 201703L
	[[nodiscard]]
#endif
	Context use() const;

	/// Draws the framebuffer image to the screen
	void draw(Vec2 position, const ShaderProgram* = nullptr) const;
	void draw(double x, double y) const;

	void draw(Mat3 modelview, const ShaderProgram* = nullptr) const;

	/// Draws a list of triangles with the framebuffer's texture on it
	void drawMesh(const std::vector<Vertex>& vertexes, const ShaderProgram* = nullptr) const;

	/// Clear the framebuffer with the color set by jngl::setBackgroundColor
	/// \deprecated use jngl::FrameBuffer::Context::clear(Color) instead
	[[deprecated("use jngl::FrameBuffer::Context::clear(Color) instead")]]
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
