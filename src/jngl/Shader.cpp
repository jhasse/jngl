// Copyright 2018-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Shader.hpp"

#include "../Shader_Impl.hpp"

#ifdef JNGL_UWP
#include "debug.hpp"
#endif

#include <boost/algorithm/string.hpp>
#include <sstream>
#include <stdexcept>

namespace jngl {

Shader::Shader(const char* source, const Type type, const char* const gles20Source [[maybe_unused]])
: impl(std::make_unique<Impl>()) {
	impl->id = glCreateShader(type == Type::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
#if defined(__APPLE__) && !defined(OPENGLES)
	std::string tmp(source);
	boost::replace_all(tmp, "#version 300 es", "#version 330");
	source = tmp.c_str();
#endif
#ifdef JNGL_UWP
	if (gles20Source) {
		source = gles20Source;
	} else {
		jngl::debugLn("WARNING: OpenGL ES 3.0 not supported on Xbox!");
	}
#endif
	glShaderSource(impl->id, 1, &source, nullptr);
	glCompileShader(impl->id);
	GLint status = GL_FALSE;
	glGetShaderiv(impl->id, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[2048];
		glGetShaderInfoLog(impl->id, sizeof(buffer), nullptr, buffer);
		throw std::runtime_error(buffer);
	}
}

Shader::Shader(const std::istream& source, const Type type)
: Shader([&source]() {
	std::stringstream buffer;
	buffer.exceptions(std::ios_base::failbit);
	buffer << source.rdbuf();
	return buffer.str();
}().c_str(), type) {
}

Shader::~Shader() {
	glDeleteShader(impl->id);
}

} // namespace jngl
