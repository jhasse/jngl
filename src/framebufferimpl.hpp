// Copyright 2011-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

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
	void Draw(double x, double y) const;
	void Clear();

private:
	GLuint fbo = 0;
	GLuint buffer = 0;
	const int height;
	Texture texture;
	GLuint systemFbo;
	GLuint systemBuffer;
#ifndef GL_VIEWPORT_BIT
	GLint viewport[4];
#endif
};

} // namespace jngl
