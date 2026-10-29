// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::ShaderBytecode
/// @file
#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace jngl {

/// A ShaderProgram's GLSL compiled at build time by jngl-shaderc, for the Vulkan (SPIR-V) or the
/// Direct3D 12 (DXBC) renderer
///
/// Direct3D 12 can't compile GLSL at runtime, so it only supports ShaderPrograms created from
/// these. jngl-shaderc generates a header with one for each program, see jngl_compile_shaders() in
/// JNGL's CMakeLists.txt:
///
/// \code
/// #include "shaders.hpp" // generated
///
/// const jngl::ShaderProgram blur(shaders::blur);
/// \endcode
struct ShaderBytecode {
	/// One of the program's uniforms, see ShaderProgram::getUniformLocation
	struct Uniform {
		std::string_view name;
		uint32_t offset;
		uint32_t size;
	};

	/// As 32-bit words
	std::span<const uint32_t> vertex;
	std::span<const uint32_t> fragment;
	std::span<const Uniform> uniforms;
	uint32_t uniformBlockSize;
	/// Whether the uniforms are push constants rather than a uniform buffer
	bool pushConstants;
};

} // namespace jngl
