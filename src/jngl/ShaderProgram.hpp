// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::ShaderProgram class
/// @file
#pragma once

#include "Rgb.hpp"

#include <memory>
#include <string>

namespace jngl {

class Shader;

/// Linked vertex and fragment shaders
class ShaderProgram {
public:
	ShaderProgram(const Shader& vertex, const Shader& fragment);
	~ShaderProgram();
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&&) noexcept;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram& operator=(ShaderProgram&&) noexcept;

	struct Impl;
	/// Lifetime object when the ShaderProgram is in use
	struct Context {
		explicit Context(const Impl&);
		~Context();
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) noexcept;
		Context& operator=(Context&&) = delete;

		/// Sets the Context's associated ShaderProgram's uniform
		/// @param location see ShaderProgram::getUniformLocation(const std::string&)
		static void setUniform(int location, int v0);
		static void setUniform(int location, float v0, float v1);
		static void setUniform(int location, float v0, float v1, float v2, float v3);

		/// sets a vec3 to the color of the Rgb object
		static void setUniform(int location, Rgb);

	private:
		static int referenceCount;
		static const Impl* activeImpl;
	};

#if __cplusplus >= 201703L
	    [[nodiscard]]
#endif
	    Context
	    use() const;

	[[nodiscard]] int getAttribLocation(const std::string& name) const;

	/// Returns the location of a uniform variable for use with Context::setUniform
	///
	/// The returned location is stable for the lifetime of the ShaderProgram and is typically
	/// queried once (e.g. in a constructor) and then cached.
	///
	/// \param name name of the uniform variable as declared in the GLSL source. Note that GLSL
	///             compilers strip away uniforms that aren't actually read by the shader, so this
	///             may fail even if the variable appears in the source.
	/// \return a non-negative location identifying the uniform
	/// \throws std::runtime_error if no active uniform with the given name exists in the linked
	///         shader program.
	[[nodiscard]] int getUniformLocation(const std::string& name) const;

private:
	std::unique_ptr<Impl> impl;
};

void setUniform(int location, float v0, float v1);

} // namespace jngl
