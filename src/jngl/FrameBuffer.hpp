// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::FrameBuffer class
/// @file
#pragma once

#include "Mat3.hpp"
#include "Pixels.hpp"
#include "ShaderProgram.hpp"
#include "Vec2.hpp"
#include "Vertex.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace jngl {

enum class TextureFilter : uint8_t {
	/// Bilinear Filtering is used when jngl::AppParameters::pixelArt is false. It smooths the
	/// image when it is scaled up.
	Bilinear,

	/// Nearest Neighbor Filtering is used when jngl::AppParameters::pixelArt is true. It
	/// preserves the pixelated look when the image is scaled up.
	NearestNeighbor,
};

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
	///
	/// \param hdr If true, the framebuffer stores floating point channels (GL_RGBA16F) instead of
	/// 8-bit ones. This avoids banding in smooth gradients and lets channel values exceed 1.0, which
	/// is useful for HDR lighting.
	FrameBuffer(Pixels width, Pixels height, bool hdr = false);

	/// Creates a framebuffer object with \a width times \a height scalable pixels
	FrameBuffer(ScaleablePixels width, ScaleablePixels height, bool hdr = false);

	/// Creates a framebuffer object with \a size[0] times \a size[1] pixels
	explicit FrameBuffer(std::array<Pixels, 2> size, bool hdr = false);

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) noexcept;
	FrameBuffer& operator=(FrameBuffer&&) noexcept;
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
	/// The projection matrix gets adjusted so that (0, 0) is in the center of the FrameBuffer. The
	/// modelview matrix is saved (i.e. pushMatrix() is called) and restored by ~Context (i.e.
	/// popMatrix() is called).
#if __cplusplus >= 201703L
	[[nodiscard]]
#endif
	Context use() const;

	/// Draws the framebuffer image to the screen
	void draw(Vec2 position, const ShaderProgram* = nullptr) const;
	void draw(double x, double y) const;

	/// Draws the framebuffer image to the screen
	///
	/// \param shaderProgram If not `nullptr`, this shader program is used instead of the default.
	void draw(Mat3 modelview, const ShaderProgram* = nullptr) const;

	/// Draws the framebuffer image to the screen with a specific texture filter
	///
	/// \param textureFilter Use this filtering for this one draw call, ignoring
	/// jngl::AppParameters::pixelArt. You'd probably want to use TextureFilter::NearestNeighbor for
	/// cases where you have a framebuffer the same size as the screen.
	/// \param shaderProgram If not `nullptr`, this shader program is used instead of the default.
	void draw(Mat3 modelview, TextureFilter textureFilter, const ShaderProgram* = nullptr) const;

	/// Draws a list of triangles with the framebuffer's texture on it
	void drawMesh(const std::vector<Vertex>& vertexes, const ShaderProgram* = nullptr) const;

	/// Clear the framebuffer with the color set by jngl::setBackgroundColor
	/// \deprecated use jngl::FrameBuffer::Context::clear(Color) instead
	[[deprecated("use jngl::FrameBuffer::Context::clear(Color) instead")]]
	static void clear();

	/// Returns the size in screen pixels
	Vec2 getSize() const;

	Pixels getPixelWidth() const;
	Pixels getPixelHeight() const;

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
