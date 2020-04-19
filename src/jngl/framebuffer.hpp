// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "Finally.hpp"
#include "Vec2.hpp"

#include <memory>

namespace jngl {

class FrameBufferImpl;

/// Image framebuffer object which can be rendered on
class FrameBuffer {
public:
	/// Creates a framebuffer object with \a width times \a height pixels
	FrameBuffer(int width, int height);

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = default;
	FrameBuffer& operator=(FrameBuffer&&) = default;
	~FrameBuffer();

	[[deprecated("use FrameBuffer::use() instead")]] void beginDraw();
	[[deprecated("use FrameBuffer::use() instead")]] void endDraw();

#if __cplusplus >= 201703L
	[[nodiscard]]
#endif
	Finally use();

	void draw(Vec2 position) const;
	void draw(double x, double y) const;
	void clear();

	/// Returns the size in screen pixels
	Vec2 getSize() const;

private:
	std::unique_ptr<FrameBufferImpl> pImpl;
};

} // namespace jngl
