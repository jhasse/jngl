// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "Vec2.hpp"

#include <memory>

namespace jngl {

class FrameBufferImpl;

/// Image framebuffer object which can be rendered on
class FrameBuffer {
public:
	FrameBuffer(int width, int height);
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = default;
	FrameBuffer& operator=(FrameBuffer&&) = default;
	~FrameBuffer();
	void beginDraw();
	void endDraw();
	void draw(Vec2 position) const;
	void draw(double x, double y) const;
	void clear();

private:
	std::unique_ptr<FrameBufferImpl> pImpl;
};

} // namespace jngl
