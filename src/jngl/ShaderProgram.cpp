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
	GLint status = GL_FALSE;
	glGetProgramiv(impl->id, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[2048];
		glGetProgramInfoLog(impl->id, sizeof(buffer), nullptr, buffer);
		throw std::runtime_error(buffer);
	}
}

Finally ShaderProgram::use() const {
	glUseProgram(impl->id);
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		switch (err) {
			case GL_INVALID_OPERATION:
				throw std::runtime_error("GL_INVALID_OPERATION");
			case GL_INVALID_ENUM:
				throw std::runtime_error("GL_INVALID_ENUM");
			case GL_INVALID_VALUE:
				throw std::runtime_error("GL_INVALID_VALUE");
			case GL_OUT_OF_MEMORY:
				throw std::runtime_error("GL_OUT_OF_MEMORY");
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				throw std::runtime_error("GL_INVALID_FRAMEBUFFER_OPERATION");
			default:
				throw std::runtime_error("Unknown OpenGL error");
		}
	}
	return Finally([]() { glUseProgram(0); });
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(impl->id);
}

} // namespace jngl
