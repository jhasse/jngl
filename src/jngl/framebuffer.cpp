// Copyright 2011-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "framebuffer.hpp"

#include "../framebufferimpl.hpp"

namespace jngl {

FrameBuffer::FrameBuffer(const int width, const int height)
: pImpl(std::make_unique<FrameBufferImpl>(width, height)) {
}

FrameBuffer::~FrameBuffer() = default;

void FrameBuffer::draw(const double x, const double y) const {
	return pImpl->draw({ x, y });
}

void FrameBuffer::draw(const Vec2 position, const ShaderProgram* const shaderProgram) const {
	return pImpl->draw(position, shaderProgram);
}

void FrameBuffer::drawMesh(const std::vector<Vertex>& vertexes,
                      const ShaderProgram* const shaderProgram) const {
	pImpl->drawMesh(vertexes, shaderProgram);
}

void FrameBuffer::beginDraw() {
	return pImpl->BeginDraw();
}

void FrameBuffer::endDraw() {
	return pImpl->EndDraw();
}

Finally FrameBuffer::use() {
	pImpl->BeginDraw();
	return Finally([this]() { pImpl->EndDraw(); });
}

void FrameBuffer::clear() {
	return pImpl->clear();
}

Vec2 FrameBuffer::getSize() const {
	return pImpl->getSize();
}

} // namespace jngl
