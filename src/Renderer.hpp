// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Internal rendering-backend abstraction.
///
/// Historically JNGL issued OpenGL/GLES calls directly from all over its code base. To make room
/// for alternative graphics APIs (currently an experimental Vulkan backend, see JNGL_VULKAN) the
/// per-frame and resource operations that genuinely differ between APIs are funnelled through the
/// abstract Renderer interface below. Exactly one Renderer is alive while a Window exists; it's
/// created by createRenderer() and reachable through getRenderer().
///
/// The backend is selected at compile time: without JNGL_VULKAN createRenderer() returns the
/// OpenGL backend (and the historic inline GL code keeps working unchanged); with JNGL_VULKAN it
/// returns the Vulkan backend.
/// @file
#pragma once

#include "jngl/Rgb.hpp"
#include "jngl/Rgba.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace jngl {

class Mat3;
class Mat4;

/// How a flat array of 2D vertices is assembled into primitives, mirroring the OpenGL draw modes
/// JNGL uses.
enum class PrimitiveType : uint8_t {
	Triangles,
	TriangleStrip,
	TriangleFan,
	Lines,
};

/// Abstract graphics backend, see Renderer.hpp.
class Renderer {
public:
	Renderer();
	virtual ~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	/// Human-readable name of the backend, e.g. "OpenGL" or "Vulkan".
	[[nodiscard]] virtual const char* name() const = 0;

	/// Begin recording a frame and clear the default render target to \a clearColor.
	///
	/// For OpenGL this performs the glClear; for Vulkan it acquires a swapchain image and begins
	/// the render pass (whose load operation clears to \a clearColor).
	virtual void beginFrame(Rgb clearColor) = 0;

	/// Finish the current frame and present it to the screen.
	///
	/// For OpenGL this swaps the double buffer; for Vulkan it ends the render pass, submits the
	/// command buffer and queues the image for presentation.
	virtual void endFrame() = 0;

	/// Set the viewport in framebuffer pixels (origin is the lower-left corner, as in OpenGL).
	virtual void setViewport(int x, int y, int width, int height) = 0;

	/// Inform the backend that the drawable surface size changed to \a width x \a height pixels.
	///
	/// For Vulkan this triggers a swapchain recreation on the next frame. For OpenGL it's a no-op.
	virtual void onResize(int width, int height) = 0;

	/// Set the projection matrix that the built-in shaders use.
	virtual void setProjection(const Mat4&) = 0;

	/// Draw a single-color primitive built from \a vertexCount 2D vertices.
	///
	/// \a xyVertices points to \a vertexCount * 2 floats (x, y interleaved) in JNGL's coordinate
	/// system. The vertices are transformed by \a modelview and the projection matrix and filled
	/// with \a color (alpha-blended). This is the path used by all of JNGL's shape drawing
	/// (jngl::drawRect, drawTriangle, drawLine, ...).
	virtual void drawColored(PrimitiveType, const float* xyVertices, std::size_t vertexCount,
	                         const Mat3& modelview, Rgba color) = 0;
};

/// Returns the active backend. Must only be called while a Window exists.
Renderer& getRenderer();

/// Returns the active backend or nullptr if no Window exists.
Renderer* getRendererIfExists();

/// Creates the compile-time selected backend and makes it the active one.
///
/// \a nativeWindow points to the platform window handle the backend needs (an SDL_Window* on the
/// SDL platform). Ownership stays with the caller.
std::unique_ptr<Renderer> createRenderer(void* nativeWindow);

} // namespace jngl
