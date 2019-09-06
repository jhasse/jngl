// Copyright 2012-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"
#include "Vec2.hpp"

#include <memory>

namespace jngl {

class FrameBufferImpl;

/// Image framebuffer object which can be rendered on
class JNGLDLL_API FrameBuffer {
public:
	FrameBuffer(int width, int height);
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
