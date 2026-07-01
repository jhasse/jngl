// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Runtime GLSL -> SPIR-V compilation for user-defined jngl::Shader programs on the Vulkan backend.
/// Built-in backend shaders are precompiled at build time; see src/vulkan/shaders/.
///
/// JNGL's public Shader API (see jngl/Shader.hpp) hands shaders to the engine as GLSL source. The
/// Vulkan backend compiles that source to SPIR-V at runtime so the public API doesn't have to
/// change.
/// @file
#pragma once

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

namespace jngl {

/// Compiles GLSL \a source for the given shader \a stage to SPIR-V. \a name is only used in error
/// messages. Throws std::runtime_error on a compilation error.
std::vector<uint32_t> compileGlslToSpirv(const char* source, VkShaderStageFlagBits stage,
                                         const char* name);

} // namespace jngl
