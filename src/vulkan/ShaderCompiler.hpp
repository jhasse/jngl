// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Runtime GLSL -> SPIR-V compilation for user-defined jngl::Shader programs on the Vulkan backend.
/// Built-in backend shaders are precompiled at build time; see src/vulkan/shaders/.
///
/// JNGL's public Shader API (see jngl/Shader.hpp) hands shaders to the engine as GLSL source. The
/// Vulkan backend compiles that source to SPIR-V at runtime so the public API doesn't have to
/// change. OpenGL-style bare non-opaque uniforms are wrapped into a std140 UBO at set 1, binding 0
/// (sampler stays at set 0, binding 0) so setUniform can update them.
/// @file
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace jngl {

/// One non-opaque uniform inside the auto-generated UBO, with its std140 byte offset/size.
struct ShaderUniform {
	std::string name;
	uint32_t offset = 0;
	uint32_t size = 0; // bytes Context::setUniform writes
};

/// Result of compileGlslToSpirv: SPIR-V words plus the UBO layout for custom uniforms (empty if the
/// shader only uses samplers / push constants).
struct CompiledShader {
	std::vector<uint32_t> spirv;
	std::vector<ShaderUniform> uniforms;
	uint32_t uniformBlockSize = 0;
};

/// Compiles GLSL \a source for the given shader \a stage to SPIR-V. \a name is only used in error
/// messages. Throws std::runtime_error on a compilation error.
CompiledShader compileGlslToSpirv(const char* source, VkShaderStageFlagBits stage,
                                  const char* name);

} // namespace jngl
