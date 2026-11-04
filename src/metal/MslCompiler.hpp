// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Turns the SPIR-V that the Vulkan backend uses into shaders for the Metal backend.
///
/// SPIRV-Cross translates the SPIR-V to the Metal Shading Language, which MetalRenderer compiles
/// when it creates a ShaderProgram, as compiling it at build time would need Xcode's optional Metal
/// Toolchain. The resources are mapped onto MetalRenderer's argument table:
///
/// - the push constant block (mvp, colors, ...) and the uniform block at set 1, binding 0 (see
///   ShaderCompiler.hpp), only one of which a program has, become buffer 0,
/// - the sampler at set 0, binding 0 becomes texture 0 and sampler 0.
///
/// Vertex inputs are attribute(location), the other inputs and outputs get user(locn<location>).
/// The entry points are called metalVertexEntryPoint and metalFragmentEntryPoint.
/// @file
#pragma once

#include "../jngl/Shader.hpp"

#include <cstdint>
#include <span>
#include <string>

namespace jngl {

constexpr const char* metalVertexEntryPoint = "vertexMain";
constexpr const char* metalFragmentEntryPoint = "fragmentMain";

/// What the MSL is for, as SPIRV-Cross translates some things differently for each
enum class MslPlatform : uint8_t {
	MacOS,
	Ios, ///< also tvOS
};

/// Translates \a spirv of a vertex or fragment shader (\a stage) to MSL for \a platform. \a name
/// is only used in error messages. Throws std::runtime_error if that fails.
std::string translateSpirvToMsl(std::span<const uint32_t> spirv, Shader::Type stage,
                                MslPlatform platform, const char* name);

} // namespace jngl
