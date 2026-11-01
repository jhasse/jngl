// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// What JNGL draws with, whichever graphics API it's been compiled for.
///
/// The backend is chosen at compile time: OpenGL by default, or the experimental Vulkan or
/// Direct3D 12 backend, see JNGL_VULKAN, JNGL_D3D12 and GpuBackend.hpp. Most of JNGL's code that
/// differs between them is in #ifdef JNGL_GPU_BACKEND, only what's the same for all of them is here.
/// @file
#pragma once

#include "jngl/Rgba.hpp"

#include <cstddef>
#include <cstdint>

namespace jngl {

class Mat3;

/// How a flat array of 2D vertices is assembled into primitives, mirroring the OpenGL draw modes
/// JNGL uses.
enum class PrimitiveType : uint8_t {
	Triangles,
	TriangleStrip,
	TriangleFan,
	Lines,
};

/// Draws a single-color primitive built from \a vertexCount 2D vertices.
///
/// \a xyVertices points to \a vertexCount * 2 floats (x, y interleaved) in JNGL's coordinate
/// system. The vertices are transformed by \a modelview and the projection matrix and filled
/// with \a color (alpha-blended). This is the path used by all of JNGL's shape drawing
/// (jngl::drawRect, drawTriangle, drawLine, ...).
void drawColored(PrimitiveType, const float* xyVertices, std::size_t vertexCount,
                 const Mat3& modelview, Rgba color);

} // namespace jngl
