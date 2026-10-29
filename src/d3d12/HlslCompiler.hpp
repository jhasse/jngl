// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Turns the SPIR-V that the Vulkan backend uses into shaders for the Direct3D 12 backend.
///
/// SPIRV-Cross translates the SPIR-V to HLSL (Shader Model 5.1), which D3DCompile compiles to DXBC.
/// The resources are mapped onto D3D12Renderer's root signature:
///
/// - the push constant block (mvp, colors, ...) becomes the root constants at b0,
/// - the sampler at set 0, binding 0 becomes the texture at t0 and its sampler at s0,
/// - the uniform block at set 1, binding 0 (see ShaderCompiler.hpp) becomes the root CBV at b1.
///
/// Vertex inputs and outputs get the semantic TEXCOORD followed by their location.
/// @file
#pragma once

#include "../jngl/Shader.hpp"

#include <cstdint>
#include <span>
#include <vector>

namespace jngl {

/// Translates \a spirv of a vertex or fragment shader (\a stage) to HLSL and compiles it. \a name
/// is only used in error messages. Throws std::runtime_error if that fails.
std::vector<uint8_t> compileSpirvToDxbc(std::span<const uint32_t> spirv, Shader::Type stage,
                                        const char* name);

} // namespace jngl
