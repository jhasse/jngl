// Copyright 2018-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::ShaderProgram class
/// @file
#pragma once

#include "Finally.hpp"

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
	ShaderProgram(ShaderProgram&&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram& operator=(ShaderProgram&&) = delete;

	struct Impl;
	/// Lifetime object when the ShaderProgram is in use
	struct Context {
		explicit Context(const Impl&);
		~Context();
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) noexcept;
		Context& operator=(Context&&) = delete;

		static void setUniform(int location, float v0, float v1);
		static void setUniform(int location, float v0, float v1, float v2, float v3);

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
	[[nodiscard]] int getUniformLocation(const std::string& name) const;

private:
	std::unique_ptr<Impl> impl;
};

void setUniform(int location, float v0, float v1);

} // namespace jngl
