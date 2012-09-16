/*
Copyright 2011-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "framebuffer.hpp"
#include "../framebufferimpl.hpp"

namespace jngl {
	FrameBuffer::FrameBuffer(const int width, const int height) : pImpl(new FrameBufferImpl(width, height)) {
	}

	FrameBuffer::~FrameBuffer() {
		delete pImpl;
	}

	void FrameBuffer::draw(int x, int y) const {
		return pImpl->Draw(x, y);
	}

	void FrameBuffer::beginDraw() {
		return pImpl->BeginDraw();
	}

	void FrameBuffer::endDraw() {
		return pImpl->EndDraw();
	}

	void FrameBuffer::clear() {
		return pImpl->Clear();
	}
}
