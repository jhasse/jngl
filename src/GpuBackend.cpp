// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#ifdef JNGL_GPU_BACKEND

#include "GpuBackend.hpp"

#include <cassert>
#include <memory>
#include <stdexcept>

namespace jngl {

namespace {
/// There's one while a Window exists, see createGpuRenderer()
std::unique_ptr<GpuRenderer> renderer;
} // namespace

#ifdef __ANDROID__
void createGpuRenderer(ANativeWindow* const window) {
#elif defined(IOS)
void createGpuRenderer(void* const window) {
#else
void createGpuRenderer(SDL_Window* const window) {
#endif
	assert(!renderer);
	renderer = std::make_unique<GpuRenderer>(window);
}

void destroyGpuRenderer() {
	renderer.reset();
}

GpuRenderer& gpuRenderer() {
	if (!renderer) {
		throw std::runtime_error("No active rendering backend. Did you call jngl::showWindow?");
	}
	return *renderer;
}

GpuRenderer* gpuRendererIfExists() {
	return renderer.get();
}

} // namespace jngl

#endif
