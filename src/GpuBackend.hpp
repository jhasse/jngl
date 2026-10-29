// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// The Renderer that's compiled in instead of OpenGL, see JNGL_GPU_BACKEND.
///
/// The Vulkan (JNGL_VULKAN) and the Direct3D 12 (JNGL_D3D12) backends offer the same API beyond
/// the Renderer interface, for textures, FrameBuffers, ShaderPrograms and so on. JNGL's code uses
/// them through the aliases below, so that it doesn't have to care which one it is.
/// @file
#pragma once

#include "Renderer.hpp"

#ifdef JNGL_VULKAN
#include "vulkan/VulkanRenderer.hpp"
#elif defined(JNGL_D3D12)
#include "d3d12/D3D12Renderer.hpp"
#else
#error "GpuBackend.hpp requires JNGL_VULKAN or JNGL_D3D12."
#endif

namespace jngl {

#ifdef JNGL_VULKAN
using GpuRenderer = VulkanRenderer;
using GpuTexture = VulkanTexture;
using GpuFramebuffer = VulkanFramebuffer;
using GpuShaderProgram = VulkanShaderProgram;
#else
using GpuRenderer = D3D12Renderer;
using GpuTexture = D3D12Texture;
using GpuFramebuffer = D3D12Framebuffer;
using GpuShaderProgram = D3D12ShaderProgram;
#endif

/// The active backend. Must only be called while a Window exists, see getRenderer().
inline GpuRenderer& gpuRenderer() {
	return static_cast<GpuRenderer&>(getRenderer());
}

/// The active backend or nullptr if no Window exists, see getRendererIfExists().
inline GpuRenderer* gpuRendererIfExists() {
	return static_cast<GpuRenderer*>(getRendererIfExists());
}

} // namespace jngl
