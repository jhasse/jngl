// Copyright 2011-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "framebuffer.hpp"
#include "../framebufferimpl.hpp"

namespace jngl {

FrameBuffer::FrameBuffer(const int width, const int height)
: pImpl(new FrameBufferImpl(width, height)) {
}

FrameBuffer::~FrameBuffer() {
	delete pImpl;
}

void FrameBuffer::draw(const double x, const double y) const {
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

} // namespace jngl
