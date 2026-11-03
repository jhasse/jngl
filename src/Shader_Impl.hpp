// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Shader.hpp"

#ifdef JNGL_GPU_BACKEND
#include <string>
#else
#include "opengl.hpp"
#endif

namespace jngl {

#ifdef JNGL_GPU_BACKEND
/// Compiled together with the program's other shader, see ShaderProgram, so that their uniforms
/// can share a layout
struct Shader::Impl {
	std::string source;
};
#else
struct Shader::Impl {
	GLuint id;

	~Impl();
};
#endif

} // namespace jngl
