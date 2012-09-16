/*
Copyright 2011 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "opengl.hpp"
#include "texture.hpp"

#include <boost/noncopyable.hpp>

namespace jngl {
class FrameBufferImpl : boost::noncopyable {
public:
	FrameBufferImpl(int width, int height);
	~FrameBufferImpl();
	void BeginDraw();
	void EndDraw();
	void Draw(int x, int y) const;
	void Clear();
private:
	GLuint fbo;
	GLuint buffer;
	const int width;
	const int height;
	Texture texture;
	GLuint systemFbo;
	GLuint systemBuffer;
#ifndef GL_VIEWPORT_BIT
	GLint viewport[4];
#endif
};
}
