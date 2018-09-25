// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Shader.hpp"

#include "../Shader_Impl.hpp"

#include <boost/algorithm/string.hpp>
#include <stdexcept>

namespace jngl {

Shader::Shader(const char* source, const Type type) : impl(std::make_unique<Impl>()) {
	impl->id = glCreateShader(type == Type::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
#if defined(__APPLE__) && !defined(OPENGLES)
	std::string tmp(source);
	boost::replace_all(tmp, "#version 300 es", "#version 330");
	source = tmp.c_str();
#endif
	glShaderSource(impl->id, 1, &source, nullptr);
	glCompileShader(impl->id);
	GLint status;
	glGetShaderiv(impl->id, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[2048];
		glGetShaderInfoLog(impl->id, sizeof(buffer), nullptr, buffer);
		throw std::runtime_error(buffer);
	}
}

Shader::~Shader() {
	glDeleteShader(impl->id);
}

} // namespace jngl
