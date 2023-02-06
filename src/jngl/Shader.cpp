// Copyright 2018-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Shader.hpp"

#include "../Shader_Impl.hpp"

#if defined (JNGL_UWP) || defined (__EMSCRIPTEN__)
#include "debug.hpp"
#endif

#include <sstream>
#include <stdexcept>

namespace jngl {

Shader::Shader(const char* source, const Type type, const char* const gles20Source [[maybe_unused]])
: impl(std::make_unique<Impl>()) {
	impl->id = glCreateShader(type == Type::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
#if defined(__APPLE__) && !defined(OPENGLES)
	std::string tmp(source);
	const std::string search = "#version 300 es";
	if (size_t pos = tmp.find(search); pos != std::string::npos) {
		 tmp.replace(pos, search.length(), "#version 330");
	}
	source = tmp.c_str();
#endif
#ifdef GLAD_GL
	if (const auto version = glGetString(GL_SHADING_LANGUAGE_VERSION)) {
		std::istringstream tmp(reinterpret_cast<const char*>(version)); // e.g. "4.60 NVIDIA"
		uint32_t major;
		uint32_t minor;
		tmp >> major;
		tmp.ignore(1); // ignore "."
		tmp >> minor;
		if (tmp && major <= 1 && minor <= 20 && gles20Source) {
			source = gles20Source;
		}
	}
#endif
#if defined (JNGL_UWP) || defined (__EMSCRIPTEN__)
	if (gles20Source) {
		source = gles20Source;
	} else {
		jngl::debugLn("WARNING: OpenGL ES 3.0 not supported on this platform!");
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

Shader::Shader(const std::istream& source, const Type type, const std::istream& gles20Source)
: Shader([&source]() {
	std::stringstream buffer;
	buffer.exceptions(std::ios_base::failbit);
	buffer << source.rdbuf();
	return buffer.str();
}().c_str(), type, [&gles20Source]() {
	std::stringstream buffer;
	buffer.exceptions(std::ios_base::failbit);
	buffer << gles20Source.rdbuf();
	return buffer.str();
}().c_str()) {
}

Shader::~Shader() {
	glDeleteShader(impl->id);
}

} // namespace jngl
