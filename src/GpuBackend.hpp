// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// The backend that's compiled in instead of OpenGL, see JNGL_GPU_BACKEND and Renderer.hpp.
///
/// The Vulkan (JNGL_VULKAN), the Direct3D 12 (JNGL_D3D12) and the Metal (JNGL_METAL) backends offer
/// the same API, for drawing, textures, FrameBuffers, ShaderPrograms and so on. JNGL's code uses
/// them through the aliases below, so that it doesn't have to care which one it is.
/// @file
#pragma once

#ifdef JNGL_VULKAN
#include "vulkan/VulkanRenderer.hpp"
#elif defined(JNGL_D3D12)
#include "d3d12/D3D12Renderer.hpp"
#elif defined(JNGL_METAL)
#include "metal/MetalRenderer.hpp"
#else
#error "GpuBackend.hpp requires JNGL_VULKAN, JNGL_D3D12 or JNGL_METAL."
#endif

namespace jngl {

#ifdef JNGL_VULKAN
using GpuRenderer = VulkanRenderer;
using GpuTexture = VulkanTexture;
using GpuFramebuffer = VulkanFramebuffer;
using GpuShaderProgram = VulkanShaderProgram;
#elif defined(JNGL_D3D12)
using GpuRenderer = D3D12Renderer;
using GpuTexture = D3D12Texture;
using GpuFramebuffer = D3D12Framebuffer;
using GpuShaderProgram = D3D12ShaderProgram;
#else
using GpuRenderer = MetalRenderer;
using GpuTexture = MetalTexture;
using GpuFramebuffer = MetalFramebuffer;
using GpuShaderProgram = MetalShaderProgram;
#endif

/// Creates the backend for \a window, which gpuRenderer() returns until destroyGpuRenderer()
#ifdef __ANDROID__
void createGpuRenderer(ANativeWindow* window);
#elif defined(IOS)
/// On iOS for \a metalLayer, a CAMetalLayer*, see MetalRenderer
void createGpuRenderer(void* metalLayer);
#else
void createGpuRenderer(SDL_Window* window);
#endif

/// Destroys the backend, before the window it renders to
void destroyGpuRenderer();

/// The backend. Must only be called while a Window exists.
GpuRenderer& gpuRenderer();

/// The backend or nullptr if no Window exists
GpuRenderer* gpuRendererIfExists();

} // namespace jngl
