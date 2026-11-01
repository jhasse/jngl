// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// What the GPU backends clip drawing to (see GpuBackend.hpp): the window's letterboxing and a
/// jngl::scissor(), which is the same for Vulkan and Direct3D 12
/// @file
#pragma once

#include <optional>

namespace jngl {

/// OpenGL-style scissor box (origin bottom-left) in the pixels of the current render target
struct ScissorBox {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

/// A rectangle in the pixels of a render target, from its top left like Vulkan's and Direct3D's
struct PixelRect {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

class GpuScissor {
public:
	/// Black bars outside a centered \a canvasW x \a canvasH area of the window's \a fbWidth x
	/// \a fbHeight pixels
	void setLetterboxing(int fbWidth, int fbHeight, int canvasW, int canvasH);

	/// What the viewport is set to, in the window's pixels
	void setFramebufferSize(int width, int height);

	/// Clips drawing to \a box (see jngl::applyScissor), or only to the window's letterboxing
	void setBox(std::optional<ScissorBox> box);

	/// Takes the swapchain's size as the window's, if setFramebufferSize() hasn't been called yet
	void sync(int swapchainWidth, int swapchainHeight);

	[[nodiscard]] bool isLetterboxed() const;

	/// See setFramebufferSize(), or 0 if it hasn't been called yet
	[[nodiscard]] int getFramebufferWidth() const;
	[[nodiscard]] int getFramebufferHeight() const;

	/// The canvas in the swapchain's \a swapchainWidth x \a swapchainHeight pixels, clipped to them
	[[nodiscard]] PixelRect letterbox(int swapchainWidth, int swapchainHeight) const;

	/// What drawing into the window (if \a window) or a FrameBuffer of \a targetWidth x
	/// \a targetHeight pixels is clipped to
	[[nodiscard]] PixelRect clip(int targetWidth, int targetHeight, bool window) const;

private:
	int framebufferWidth = 0;
	int framebufferHeight = 0;
	int canvasWidth = 0;
	int canvasHeight = 0;
	bool letterboxing = false;
	std::optional<ScissorBox> userBox; ///< see setBox()
};

} // namespace jngl
