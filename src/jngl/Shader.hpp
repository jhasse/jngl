// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "dll.hpp"

#include <memory>

/// JNGL's main namespace
namespace jngl {

/// Vertex or fragment GLSL shader
class Shader {
public:
	/// Type of a Shader. To link a ShaderProgram, one shader of each type is needed.
	enum class Type {
		VERTEX,
		FRAGMENT,
	};

	JNGLDLL_API Shader(const char* source, Type);
	JNGLDLL_API ~Shader();
	Shader(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader& operator=(Shader&&) = delete;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;

	friend class ShaderProgram;
};

} // namespace jngl
