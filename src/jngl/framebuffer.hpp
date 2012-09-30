/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#pragma GCC visibility push(default)
namespace jngl {
	class FrameBufferImpl;

	class FrameBuffer {
	public:
		FrameBuffer(int width, int height);
		~FrameBuffer();
		void beginDraw();
		void endDraw();
		void draw(int x, int y) const;
		void clear();
	private:
		FrameBuffer(const FrameBuffer&);
		FrameBuffer& operator=(const FrameBuffer&);
		FrameBufferImpl* pImpl;
	};
}
#pragma GCC visibility pop