// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Renderer.hpp"

#include <cassert>
#include <stdexcept>

#ifdef JNGL_VULKAN
#include "vulkan/VulkanRenderer.hpp"
#else
#include "OpenGLRenderer.hpp"
#endif

namespace jngl {

namespace {
/// The currently active backend. There's at most one at a time; a Renderer registers itself here
/// on construction and unregisters on destruction.
Renderer* activeRenderer = nullptr;
} // namespace

Renderer::Renderer() {
	activeRenderer = this;
}

Renderer::~Renderer() {
	if (activeRenderer == this) {
		activeRenderer = nullptr;
	}
}

Renderer& getRenderer() {
	if (!activeRenderer) {
		throw std::runtime_error("No active rendering backend. Did you call jngl::showWindow?");
	}
	return *activeRenderer;
}

Renderer* getRendererIfExists() {
	return activeRenderer;
}

std::unique_ptr<Renderer> createRenderer(void* const nativeWindow) {
	assert(nativeWindow);
#ifdef JNGL_VULKAN
	return std::make_unique<VulkanRenderer>(nativeWindow);
#else
	return std::make_unique<OpenGLRenderer>(nativeWindow);
#endif
}

} // namespace jngl
