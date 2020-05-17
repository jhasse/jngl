// Copyright 2011-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Vec2.hpp"
#include "texture.hpp"

namespace jngl {

class FrameBufferImpl {
public:
	FrameBufferImpl(int width, int height);
	FrameBufferImpl(const FrameBufferImpl&) = delete;
	FrameBufferImpl& operator=(const FrameBufferImpl&) = delete;
	FrameBufferImpl(FrameBufferImpl&&) = delete;
	FrameBufferImpl& operator=(FrameBufferImpl&&) = delete;
	~FrameBufferImpl();
	void BeginDraw();
	void EndDraw();
	void draw(Vec2 pos, const ShaderProgram* = nullptr) const;
	void drawMesh(const std::vector<Vertex>& vertexes,
	              const ShaderProgram* const shaderProgram) const;
	static void clear();
	Vec2 getSize() const;

private:
	GLuint fbo = 0;
	GLuint buffer = 0;
	const int height;
	Texture texture;
	bool letterboxing;
	GLuint systemFbo;
	GLuint systemBuffer;
#if !defined(GL_VIEWPORT_BIT) || defined(__APPLE__)
	GLint viewport[4];
#endif
};

} // namespace jngl
