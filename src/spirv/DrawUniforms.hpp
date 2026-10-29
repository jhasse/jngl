// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// The uniforms that the Vulkan and Direct3D 12 renderers set for each draw, like the OpenGL
/// backend does with glUniform: modelview and projection, and the colors and sizes of the built-in
/// shaders (see BuiltinShaders.hpp). They're written into a copy of the program's std140 block,
/// see ShaderCompiler.hpp, which is then pushed or copied into a UBO.
/// @file
#pragma once

#include "../jngl/Mat3.hpp"
#include "../jngl/Mat4.hpp"
#include "../jngl/ShaderBytecode.hpp"
#include "ShaderCompiler.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace jngl {

/// Where a program has the uniforms that are set for each draw, or -1 for those it doesn't have
struct DrawUniformOffsets {
	int32_t modelview = -1;
	int32_t projection = -1;
	int32_t color = -1;
	int32_t lerpColor = -1;
	int32_t lerpFactors = -1;
	int32_t size = -1;
	int32_t cornerRadii = -1;
};

/// A program's uniform block, see CompiledProgram
struct UniformBlock {
	uint32_t size = 0;
	bool pushConstants = false;
	DrawUniformOffsets offsets;
};

/// \a uniforms has elements with a name and an offset, e.g. ShaderUniform
template <class Uniforms>
UniformBlock uniformBlock(const Uniforms& uniforms, const uint32_t size, const bool pushConstants) {
	UniformBlock block{ size, pushConstants, {} };
	const auto find = [&uniforms](const char* const name) {
		for (const auto& uniform : uniforms) {
			if (uniform.name == name) {
				return static_cast<int32_t>(uniform.offset);
			}
		}
		return int32_t{ -1 };
	};
	block.offsets = { find("modelview"),  find("projection"),  find("color"),
		              find("lerpColor"),  find("lerpFactors"), find("size"),
		              find("cornerRadii") };
	return block;
}

/// The uniforms of a ShaderBytecode, like CompiledProgram::uniforms
inline std::vector<ShaderUniform> shaderUniforms(const ShaderBytecode& bytecode) {
	std::vector<ShaderUniform> uniforms;
	uniforms.reserve(bytecode.uniforms.size());
	for (const ShaderBytecode::Uniform& uniform : bytecode.uniforms) {
		uniforms.push_back({ std::string(uniform.name), uniform.offset, uniform.size });
	}
	return uniforms;
}

/// Writes \a values at \a offset into \a data, if it isn't -1
template <size_t N>
void writeUniform(std::vector<uint8_t>& data, const int32_t offset,
                  const std::array<float, N>& values) {
	if (offset >= 0) {
		std::memcpy(data.data() + offset, values.data(), sizeof(values));
	}
}

/// A mat3 takes three columns of a vec4 each in std140
inline void writeUniform(std::vector<uint8_t>& data, const int32_t offset, const Mat3& matrix) {
	if (offset >= 0) {
		for (size_t column = 0; column < 3; ++column) {
			std::memcpy(data.data() + offset + column * 16, matrix.data + column * 3,
			            3 * sizeof(float));
		}
	}
}

inline void writeUniform(std::vector<uint8_t>& data, const int32_t offset, const Mat4& matrix) {
	if (offset >= 0) {
		std::memcpy(data.data() + offset, matrix.data, sizeof(matrix.data));
	}
}

} // namespace jngl
