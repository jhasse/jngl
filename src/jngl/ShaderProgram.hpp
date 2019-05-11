// Copyright 2018-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "Finally.hpp"

#include <memory>

namespace jngl {

class Shader;

/// Linked vertex and fragment shaders
class ShaderProgram {
public:
	JNGLDLL_API ShaderProgram(const Shader& vertex, const Shader& fragment);
	JNGLDLL_API ~ShaderProgram();
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram& operator=(ShaderProgram&&) = delete;

	struct Impl;
	/// Lifetime object when the ShaderProgram is in use
	struct Context {
		Context(const Impl&);
		~Context();
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) noexcept;
		Context& operator=(Context&&) = delete;

		void setUniform(int location, float v0, float v1);
		void setUniform(int location, float v0, float v1, float v2, float v3);

	private:
		static int referenceCount;
		static const Impl* activeImpl;
	};

	JNGLDLL_API
#if __cplusplus >= 201703L
	    [[nodiscard]]
#endif
	    Context
	    use() const;

	int getAttribLocation(const std::string& name) const;
	int getUniformLocation(const std::string& name) const;

private:
	std::unique_ptr<Impl> impl;
};

void setUniform(int location, float v0, float v1);

} // namespace jngl
