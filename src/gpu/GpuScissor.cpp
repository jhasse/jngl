// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "GpuScissor.hpp"

#include <algorithm>

namespace jngl {

namespace {
/// \a glX, \a glY, \a glW, \a glH from the bottom left of what's \a targetHeight pixels high
PixelRect fromBottomLeft(const int glX, const int glY, const int glW, const int glH,
                         const int targetHeight) {
	return { std::max(glX, 0), std::max(targetHeight - glY - glH, 0), std::max(glW, 0),
		     std::max(glH, 0) };
}

/// Empty if they don't overlap
PixelRect intersect(const PixelRect a, const PixelRect b) {
	const int x1 = std::max(a.x, b.x);
	const int y1 = std::max(a.y, b.y);
	const int x2 = std::min(a.x + a.width, b.x + b.width);
	const int y2 = std::min(a.y + a.height, b.y + b.height);
	if (x2 > x1 && y2 > y1) {
		return { x1, y1, x2 - x1, y2 - y1 };
	}
	return {};
}
} // namespace

void GpuScissor::setLetterboxing(const int fbWidth, const int fbHeight, const int canvasW,
                                 const int canvasH) {
	framebufferWidth = fbWidth;
	framebufferHeight = fbHeight;
	canvasWidth = canvasW;
	canvasHeight = canvasH;
	letterboxing = canvasWidth != framebufferWidth || canvasHeight != framebufferHeight;
}

void GpuScissor::setFramebufferSize(const int width, const int height) {
	framebufferWidth = width;
	framebufferHeight = height;
}

void GpuScissor::setBox(const std::optional<ScissorBox> box) {
	userBox = box;
}

void GpuScissor::sync(const int swapchainWidth, const int swapchainHeight) {
	if (canvasWidth <= 0 || canvasHeight <= 0 || swapchainWidth == 0) {
		return;
	}
	if (framebufferWidth <= 0 || framebufferHeight <= 0) {
		framebufferWidth = swapchainWidth;
		framebufferHeight = swapchainHeight;
	}
	letterboxing = canvasWidth != framebufferWidth || canvasHeight != framebufferHeight;
}

bool GpuScissor::isLetterboxed() const {
	return letterboxing;
}

int GpuScissor::getFramebufferWidth() const {
	return framebufferWidth;
}

int GpuScissor::getFramebufferHeight() const {
	return framebufferHeight;
}

PixelRect GpuScissor::letterbox(const int swapchainWidth, const int swapchainHeight) const {
	// Centered in the viewport, which is at the bottom left of the swapchain's image
	const int x = (framebufferWidth - canvasWidth) / 2;
	const int y = (framebufferHeight - canvasHeight) / 2;
	return intersect(fromBottomLeft(x, y, canvasWidth, canvasHeight, swapchainHeight),
	                 { 0, 0, swapchainWidth, swapchainHeight });
}

PixelRect GpuScissor::clip(const int targetWidth, const int targetHeight, const bool window) const {
	PixelRect base{ 0, 0, targetWidth, targetHeight };
	if (window && letterboxing) {
		base = letterbox(targetWidth, targetHeight);
	}
	if (!userBox) {
		return base;
	}
	if (window) {
		return intersect(base, fromBottomLeft(userBox->x, userBox->y, userBox->width,
		                                      userBox->height, targetHeight));
	}
	// FrameBuffers are drawn into upside down, like OpenGL FBO textures, so their rows already
	// start at the bottom
	return intersect(base, { std::max(userBox->x, 0), std::max(userBox->y, 0),
	                         std::max(userBox->width, 0), std::max(userBox->height, 0) });
}

} // namespace jngl
