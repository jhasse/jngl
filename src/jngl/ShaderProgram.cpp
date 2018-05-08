// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "ShaderProgram.hpp"

#include "../Shader_Impl.hpp"

namespace jngl {

struct ShaderProgram::Impl {
	GLuint id;
};

ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& fragment)
: impl(std::make_unique<Impl>()) {
	impl->id = glCreateProgram();
	glAttachShader(impl->id, vertex.impl->id);
	glAttachShader(impl->id, fragment.impl->id);
	glLinkProgram(impl->id);
}

Finally ShaderProgram::use() const {
	glUseProgram(impl->id);
	return Finally([]() { glUseProgram(0); });
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(impl->id);
}

} // namespace jngl
