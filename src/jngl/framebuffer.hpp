/*
Copyright 2012-2014 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "dll.hpp"

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif

namespace jngl {
	class FrameBufferImpl;

	class JNGLDLL_API FrameBuffer {
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
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
