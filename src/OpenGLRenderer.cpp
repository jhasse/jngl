// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifndef JNGL_VULKAN

#include "OpenGLRenderer.hpp"

#include "ShaderCache.hpp"
#include "jngl/Mat3.hpp"
#include "jngl/Mat4.hpp"
#include "main.hpp"
#include "opengl.hpp"
#include "windowptr.hpp"

namespace jngl {

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

OpenGLRenderer::OpenGLRenderer(void* /*nativeWindow*/) {
	// The OpenGL context is created and the GL function pointers are loaded by the platform-specific
	// Window code (see src/sdl/window.cpp). Nothing to do here.
}

OpenGLRenderer::~OpenGLRenderer() = default;

const char* OpenGLRenderer::name() const {
	return "OpenGL";
}

void OpenGLRenderer::beginFrame(const Rgb clearColor) {
	glClearColor(clearColor.getRed(), clearColor.getGreen(), clearColor.getBlue(), 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderer::endFrame() {
	pWindow->SwapBuffers();
}

void OpenGLRenderer::setViewport(const int x, const int y, const int width, const int height) {
	glViewport(x, y, width, height);
}

void OpenGLRenderer::onResize(int /*width*/, int /*height*/) {
	// The default framebuffer resizes with the window automatically.
}

void OpenGLRenderer::setProjection(const Mat4& projection) {
	opengl::projection = projection;
}

void OpenGLRenderer::drawColored(const PrimitiveType type, const float* const xyVertices,
                                 const std::size_t vertexCount, const Mat3& modelview,
                                 const Rgba color) {
	opengl::bindVertexArray(opengl::vaoStream);
	auto context = ShaderCache::handle().useSimpleShaderProgram(modelview, color);
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // the VAO does NOT save the VBO binding
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexCount * 2 * sizeof(float)),
	             xyVertices, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(toGlPrimitive(type), 0, static_cast<GLsizei>(vertexCount));
}

} // namespace jngl

#endif
