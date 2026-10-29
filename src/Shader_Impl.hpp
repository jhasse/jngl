// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Shader.hpp"

#ifdef JNGL_GPU_BACKEND
#include "spirv/ShaderCompiler.hpp"

#include <cstdint>
#include <vector>
#else
#include "opengl.hpp"
#endif

namespace jngl {

#ifdef JNGL_GPU_BACKEND
struct Shader::Impl {
	std::vector<uint32_t> spirv;
	std::vector<ShaderUniform> uniforms;
	uint32_t uniformBlockSize = 0;
};
#else
struct Shader::Impl {
	GLuint id;

	~Impl();
};
#endif

} // namespace jngl
