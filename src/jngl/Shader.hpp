// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <memory>

namespace jngl {

class Shader {
public:
	enum class Type {
		VERTEX,
		FRAGMENT,
	};

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
