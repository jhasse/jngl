// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Renderer.hpp"

#ifdef JNGL_GPU_BACKEND
#include "GpuBackend.hpp"
#else
#include "ShaderCache.hpp"
#include "jngl/Mat3.hpp"
#include "opengl.hpp"
#endif

namespace jngl {

#ifdef JNGL_GPU_BACKEND

void drawColored(const PrimitiveType type, const float* const xyVertices,
                 const std::size_t vertexCount, const Mat3& modelview, const Rgba color) {
	gpuRenderer().drawColored(type, xyVertices, vertexCount, modelview, color);
}

#else

namespace {
GLenum toGlPrimitive(const PrimitiveType type) {
	switch (type) {
	case PrimitiveType::Triangles:
		return GL_TRIANGLES;
	case PrimitiveType::TriangleStrip:
		return GL_TRIANGLE_STRIP;
	case PrimitiveType::TriangleFan:
		return GL_TRIANGLE_FAN;
	case PrimitiveType::Lines:
		return GL_LINES;
	}
	return GL_TRIANGLES;
}
} // namespace

void drawColored(const PrimitiveType type, const float* const xyVertices,
                 const std::size_t vertexCount, const Mat3& modelview, const Rgba color) {
	opengl::bindVertexArray(opengl::vaoStream);
	auto context = ShaderCache::handle().useSimpleShaderProgram(modelview, color);
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // the VAO does NOT save the VBO binding
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexCount * 2 * sizeof(float)),
	             xyVertices, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(toGlPrimitive(type), 0, static_cast<GLsizei>(vertexCount));
}

#endif

} // namespace jngl
