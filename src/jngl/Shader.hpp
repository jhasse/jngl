// Copyright 2018-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

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

	/// Creates a vertex or fragment shader by compiling it.
	/// \param source Source code using either GLSL v3.30 or GLSL ES v3.00.
	Shader(const char* source, Type);
	~Shader();
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
