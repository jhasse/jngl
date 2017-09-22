// Copyright 2011-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "opengl.hpp"
#include "texture.hpp"

namespace jngl {
class FrameBufferImpl {
public:
	FrameBufferImpl(int width, int height);
	FrameBufferImpl(const FrameBufferImpl&) = delete;
	FrameBufferImpl& operator=(const FrameBufferImpl&) = delete;
	~FrameBufferImpl();
	void BeginDraw();
	void EndDraw();
	void Draw(int x, int y) const;
	void Clear();
private:
	GLuint fbo;
	GLuint buffer;
	const int height;
	Texture texture;
	GLuint systemFbo;
	GLuint systemBuffer;
#ifndef GL_VIEWPORT_BIT
	GLint viewport[4];
#endif
};
}
