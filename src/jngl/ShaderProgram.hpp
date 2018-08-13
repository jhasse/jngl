// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "Finally.hpp"

#include <memory>

namespace jngl {

class Shader;

class ShaderProgram {
public:
	JNGLDLL_API ShaderProgram(const Shader& vertex, const Shader& fragment);
	JNGLDLL_API ~ShaderProgram();
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram& operator=(ShaderProgram&&) = delete;
	JNGLDLL_API [[nodiscard]] Finally use() const;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
