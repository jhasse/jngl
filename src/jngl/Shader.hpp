// Copyright 2018-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include <memory>

/// JNGL's main namespace
namespace jngl {

/// Fragment or vertex GLSL shader
class Shader {
public:
	/// Type of a Shader. To link a ShaderProgram, one shader of each type is needed.
	enum class Type {
		VERTEX,
		FRAGMENT,
	};

	/// Creates a vertex or fragment shader by compiling it.
	/// \param source Source code using either GLSL v3.30 or GLSL ES v3.00.
	/// \param gles20Source Source code using GLSL v1.00 for OpenGL ES 2.0
	Shader(const char* source, Type, const char* gles20Source = nullptr);

	/// \overload
	///
	/// \code
	/// jngl::Shader foo(jngl::readAsset("foo.frag"), jngl::Shader::Type::FRAGMENT);
	/// \endcode
	Shader(const std::istream& source, Type);

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
