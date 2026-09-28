// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// OpenGL implementation of the Renderer interface, see Renderer.hpp. This is the default backend;
/// it's a thin wrapper around the historic inline OpenGL code so that getRenderer() is usable in
/// the OpenGL build as well.
/// @file
#pragma once

#include "Renderer.hpp"

namespace jngl {

class OpenGLRenderer final : public Renderer {
public:
	/// \a nativeWindow is the SDL_Window* (unused; the GL context is created by the Window itself).
	explicit OpenGLRenderer(void* nativeWindow);
	~OpenGLRenderer() override;

	[[nodiscard]] const char* name() const override;
	void beginFrame(Rgb clearColor) override;
	void endFrame() override;
	void setViewport(int x, int y, int width, int height) override;
	void onResize(int width, int height) override;
	void setProjection(const Mat4&) override;
	void drawColored(PrimitiveType, const float* xyVertices, std::size_t vertexCount,
	                 const Mat3& modelview, Rgba color) override;
};

} // namespace jngl
