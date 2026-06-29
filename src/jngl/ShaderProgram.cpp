// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "ShaderProgram.hpp"

#include "../App.hpp"
#include "../Shader_Impl.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

#ifdef JNGL_VULKAN
#include "../Renderer.hpp"
#include "../vulkan/VulkanRenderer.hpp"
#endif

namespace jngl {

int ShaderProgram::Context::referenceCount = 0;
const ShaderProgram::Impl* ShaderProgram::Context::activeImpl = nullptr;

#ifdef JNGL_VULKAN

namespace {
VulkanRenderer& vulkanRenderer() {
	return static_cast<VulkanRenderer&>(getRenderer());
}
} // namespace

struct ShaderProgram::Impl {
	std::unique_ptr<VulkanShaderProgram> program;

	~Impl() {
		if (program) {
			if (auto* renderer = getRendererIfExists()) {
				static_cast<VulkanRenderer*>(renderer)->destroyShaderProgram(*program);
			}
		}
	}
};

ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& fragment)
: impl(std::make_unique<Impl>()) {
	impl->program = vulkanRenderer().createShaderProgram(vertex.impl->spirv, fragment.impl->spirv);
	App::instance().registerShaderProgram(this);
}

ShaderProgram::Context ShaderProgram::use() const {
	return Context(*impl);
}

int ShaderProgram::getAttribLocation(const std::string& /*name*/) const {
	// Attribute locations are fixed by the built-in sprite vertex shader on Vulkan.
	return 0;
}

int ShaderProgram::getUniformLocation(const std::string& /*name*/) const {
	// Custom uniforms aren't supported on the Vulkan backend yet; the built-in modelview/projection
	// are passed as push constants automatically.
	return -1;
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
#ifdef JNGL_VULKAN
		vulkanRenderer().setActiveShaderProgram(impl.program.get());
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
#ifdef JNGL_VULKAN
	if (referenceCount == 0) {
		activeImpl = nullptr;
		if (auto* renderer = getRendererIfExists()) {
			static_cast<VulkanRenderer*>(renderer)->setActiveShaderProgram(nullptr);
		}
	}
#endif
}

void ShaderProgram::Context::setUniform(const int location, const int v0) {
	assert(referenceCount >= 0);
#ifdef JNGL_VULKAN
	(void)location;
	(void)v0; // custom uniforms aren't supported on Vulkan yet
#else
	glUniform1i(location, v0);
#endif
}

void ShaderProgram::Context::setUniform(const int location, const float v0, const float v1) {
	assert(referenceCount >= 0);
#ifdef JNGL_VULKAN
	(void)location;
	(void)v0;
	(void)v1;
#else
	glUniform2f(location, v0, v1);
#endif
}

void ShaderProgram::Context::setUniform(const int location, const float v0, const float v1,
                                        const float v2, const float v3) {
	assert(referenceCount >= 0);
#ifdef JNGL_VULKAN
	(void)location;
	(void)v0;
	(void)v1;
	(void)v2;
	(void)v3;
#else
	glUniform4f(location, v0, v1, v2, v3);
#endif
}

void ShaderProgram::Context::setUniform(const int location, const Rgb color) {
	assert(referenceCount >= 0);
#ifdef JNGL_VULKAN
	(void)location;
	(void)color;
#else
	glUniform3f(location, color.getRed(), color.getGreen(), color.getBlue());
#endif
}

} // namespace jngl
