// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// GLSL -> SPIR-V compilation for the Vulkan and Direct3D 12 backends: at runtime for user-defined
/// jngl::ShaderPrograms, and at build time for the built-in ones by jngl-shaderc (see
/// BuiltinShaders.hpp).
///
/// JNGL's public Shader API (see jngl/Shader.hpp) hands shaders to the engine as GLSL source,
/// written for OpenGL. These backends compile that source to SPIR-V so the public API doesn't have
/// to change (the Direct3D 12 backend then translates the SPIR-V to HLSL, see compileSpirvToDxbc).
/// The bare non-opaque uniforms of a program's shaders are wrapped into one std140 block so
/// setUniform can update them: push constants if they fit, otherwise a UBO at set 1, binding 0.
/// Samplers are at set 0, binding 0.
/// @file
#pragma once

#include "../jngl/Shader.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace jngl {

/// The uniforms of a program are push constants if they need at most this many bytes, which is
/// what every Vulkan device supports
constexpr uint32_t maxPushConstantsSize = 128;

/// One non-opaque uniform inside the auto-generated block, with its std140 byte offset/size.
///
/// Of an array, there's one for each element: the first one has the array's name, the others
/// "name[1]", "name[2]" and so on, see ShaderProgram::getUniformLocation.
struct ShaderUniform {
	std::string name;
	uint32_t offset = 0;
	uint32_t size = 0; // bytes Context::setUniform writes
};

/// Result of compileGlslProgram: SPIR-V words of both shaders plus the layout of their uniforms
/// (empty if they only use samplers).
struct CompiledProgram {
	std::vector<uint32_t> vertexSpirv;
	std::vector<uint32_t> fragmentSpirv;
	std::vector<ShaderUniform> uniforms;
	uint32_t uniformBlockSize = 0;
	/// Whether the uniforms are push constants, see maxPushConstantsSize, rather than the UBO
	bool pushConstants = false;
};

/// Compiles a program's GLSL shaders to SPIR-V, with the uniforms of both in the same block.
/// Throws std::runtime_error on a compilation error.
CompiledProgram compileGlslProgram(const char* vertexSource, const char* fragmentSource);

/// Throws std::runtime_error if GLSL \a source for the given shader \a stage doesn't compile, see
/// jngl::Shader. \a name is only used in error messages.
void checkGlsl(const char* source, Shader::Type stage, const char* name);

} // namespace jngl
