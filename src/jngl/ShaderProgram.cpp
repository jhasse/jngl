// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "ShaderProgram.hpp"

#include "../App.hpp"
#include "../Shader_Impl.hpp"

#include <array>
#include <cassert>
#include <stdexcept>
#include <string_view>

#ifdef JNGL_GPU_BACKEND
#include "../GpuBackend.hpp"
#ifndef JNGL_D3D12
#include "../spirv/ShaderCompiler.hpp"
#endif
#endif

namespace jngl {

int ShaderProgram::Context::referenceCount = 0;
const ShaderProgram::Impl* ShaderProgram::Context::activeImpl = nullptr;

#ifdef JNGL_GPU_BACKEND

struct ShaderProgram::Impl {
	std::unique_ptr<GpuShaderProgram> program;

	~Impl() {
		if (program) {
			if (auto* renderer = gpuRendererIfExists()) {
				renderer->destroyShaderProgram(*program);
			}
		}
	}
};

ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& fragment)
: impl(std::make_unique<Impl>()) {
#ifdef JNGL_D3D12
	(void)vertex;
	(void)fragment;
	throw std::runtime_error(
	    "The Direct3D 12 renderer can't compile shaders at runtime. Compile "
	    "them with jngl-shaderc and use ShaderProgram(const ShaderBytecode&).");
#else
	impl->program = gpuRenderer().createShaderProgram(
	    compileGlslProgram(vertex.impl->source.c_str(), fragment.impl->source.c_str()));
	App::instance().registerShaderProgram(this);
#endif
}

ShaderProgram::ShaderProgram(const ShaderBytecode& bytecode) : impl(std::make_unique<Impl>()) {
	impl->program = gpuRenderer().createShaderProgram(bytecode);
	App::instance().registerShaderProgram(this);
}

ShaderProgram::Context ShaderProgram::use() const {
	return Context(*impl);
}

int ShaderProgram::getAttribLocation(const std::string& /*name*/) const {
	// Attribute locations are fixed by the built-in sprite vertex shader on the GPU backends.
	return 0;
}

int ShaderProgram::getUniformLocation(const std::string& name) const {
	// Like on OpenGL, the first element of an array is also its name alone, see ShaderUniform
	const std::string_view element0 = "[0]";
	const std::string_view search =
	    name.ends_with(element0) ? std::string_view(name).substr(0, name.size() - element0.size())
	                             : std::string_view(name);
	const auto& uniforms = impl->program->uniforms;
	for (int i = 0; i < static_cast<int>(uniforms.size()); ++i) {
		if (uniforms[static_cast<size_t>(i)].name == search) {
			return i;
		}
	}
	throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
}

#else

struct ShaderProgram::Impl {
	GLuint id;

	~Impl() {
		glDeleteProgram(id);
	}
};

Shader::Impl::~Impl() {
	glDeleteShader(id);
}

ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& fragment)
: impl(std::make_unique<Impl>()) {
	impl->id = glCreateProgram();
	glAttachShader(impl->id, vertex.impl->id);
	glAttachShader(impl->id, fragment.impl->id);
	glLinkProgram(impl->id);
	GLint status = GL_FALSE;
	glGetProgramiv(impl->id, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		std::array<char, 2048> buffer{};
		glGetProgramInfoLog(impl->id, static_cast<GLsizei>(std::size(buffer)), nullptr,
		                    buffer.data());
		throw std::runtime_error(buffer.data());
	}
	const auto tmp = use();
	glUniformMatrix4fv(getUniformLocation("projection"), 1, GL_FALSE, opengl::projection.data);
	App::instance().registerShaderProgram(this);
}

ShaderProgram::ShaderProgram(const ShaderBytecode& /*bytecode*/) {
	throw std::runtime_error("The OpenGL renderer doesn't support ShaderBytecode, compile the GLSL "
	                         "at runtime with ShaderProgram(const Shader&, const Shader&).");
}

ShaderProgram::Context ShaderProgram::use() const {
	return Context(*impl);
}

int ShaderProgram::getAttribLocation(const std::string& name) const {
	int location = glGetAttribLocation(impl->id, name.c_str());
	assert(location != -1);
	return location;
}

int ShaderProgram::getUniformLocation(const std::string& name) const {
	int location = glGetUniformLocation(impl->id, name.c_str());
	if (location == -1) {
		throw std::runtime_error("Uniform '" + name + "' not found in shader program.");
	}
	return location;
}

#endif

ShaderProgram::~ShaderProgram() {
	if (App::self) {
		App::self->unregisterShaderProgram(this);
	}
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept : impl(std::move(other.impl)) {
	if (App::self) {
		App::self->unregisterShaderProgram(&other);
		if (impl) {
			App::self->registerShaderProgram(this);
		}
	}
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
	if (this == &other) {
		return *this;
	}
	if (impl && App::self) {
		App::self->unregisterShaderProgram(this);
	}
	impl = std::move(other.impl);
	if (App::self) {
		App::self->unregisterShaderProgram(&other);
		if (impl) {
			App::self->registerShaderProgram(this);
		}
	}
	return *this;
}

ShaderProgram::Context::Context(const ShaderProgram::Impl& impl) {
	if (referenceCount > 0) {
		if (activeImpl != &impl) {
			throw std::runtime_error("A different ShaderProgram is already in use.");
		}
	} else {
#ifdef JNGL_GPU_BACKEND
		gpuRenderer().setActiveShaderProgram(impl.program.get());
#else
		glUseProgram(impl.id);
#endif
	}
	++referenceCount;
	activeImpl = &impl;
}

ShaderProgram::Context::Context(Context&&) noexcept {
	++referenceCount;
}

ShaderProgram::Context::~Context() {
	--referenceCount;
	assert(referenceCount >= 0);
#ifdef JNGL_GPU_BACKEND
	if (referenceCount == 0) {
		activeImpl = nullptr;
		if (auto* renderer = gpuRendererIfExists()) {
			renderer->setActiveShaderProgram(nullptr);
		}
	}
#endif
}

void ShaderProgram::Context::setUniform(const int location, const int v0) {
	assert(referenceCount >= 0);
#ifdef JNGL_GPU_BACKEND
	assert(activeImpl);
	const auto& u = activeImpl->program->uniforms.at(static_cast<size_t>(location));
	assert(u.size >= sizeof(v0));
	gpuRenderer().setActiveProgramUniform(u.offset, &v0, sizeof(v0));
#else
	glUniform1i(location, v0);
#endif
}

void ShaderProgram::Context::setUniform(const int location, const float v0) {
	assert(referenceCount >= 0);
#ifdef JNGL_GPU_BACKEND
	assert(activeImpl);
	const auto& u = activeImpl->program->uniforms.at(static_cast<size_t>(location));
	assert(u.size >= sizeof(v0));
	gpuRenderer().setActiveProgramUniform(u.offset, &v0, sizeof(v0));
#else
	glUniform1f(location, v0);
#endif
}

void ShaderProgram::Context::setUniform(const int location, const float v0, const float v1) {
	assert(referenceCount >= 0);
#ifdef JNGL_GPU_BACKEND
	assert(activeImpl);
	const auto& u = activeImpl->program->uniforms.at(static_cast<size_t>(location));
	const float values[2] = { v0, v1 };
	assert(u.size >= sizeof(values));
	gpuRenderer().setActiveProgramUniform(u.offset, values, sizeof(values));
#else
	glUniform2f(location, v0, v1);
#endif
}

void ShaderProgram::Context::setUniform(const int location, const float v0, const float v1,
                                        const float v2, const float v3) {
	assert(referenceCount >= 0);
#ifdef JNGL_GPU_BACKEND
	assert(activeImpl);
	const auto& u = activeImpl->program->uniforms.at(static_cast<size_t>(location));
	const float values[4] = { v0, v1, v2, v3 };
	assert(u.size >= sizeof(values));
	gpuRenderer().setActiveProgramUniform(u.offset, values, sizeof(values));
#else
	glUniform4f(location, v0, v1, v2, v3);
#endif
}

void ShaderProgram::Context::setUniform(const int location, const Rgb color) {
	assert(referenceCount >= 0);
#ifdef JNGL_GPU_BACKEND
	assert(activeImpl);
	const auto& u = activeImpl->program->uniforms.at(static_cast<size_t>(location));
	const float values[3] = { color.getRed(), color.getGreen(), color.getBlue() };
	assert(u.size >= sizeof(values));
	gpuRenderer().setActiveProgramUniform(u.offset, values, sizeof(values));
#else
	glUniform3f(location, color.getRed(), color.getGreen(), color.getBlue());
#endif
}

} // namespace jngl
