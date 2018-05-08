// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Shader.hpp"

#include "../Shader_Impl.hpp"

namespace jngl {

Shader::Shader(const char* const source, const Type type) : impl(std::make_unique<Impl>()) {
	impl->id = glCreateShader(type == Type::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(impl->id, 1, &source, nullptr);
	glCompileShader(impl->id);
	GLint status;
	glGetShaderiv(impl->id, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[2048];
		glGetShaderInfoLog(impl->id, 512, nullptr, buffer);
		throw std::runtime_error(buffer);
	}
}

Shader::~Shader() {
	glDeleteShader(impl->id);
}

} // namespace jngl
