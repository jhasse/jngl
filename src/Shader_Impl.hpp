// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Shader.hpp"

#ifdef JNGL_VULKAN
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#else
#include "opengl.hpp"
#endif

namespace jngl {

#ifdef JNGL_VULKAN
struct Shader::Impl {
	std::vector<uint32_t> spirv;
	VkShaderStageFlagBits stage;
};
#else
struct Shader::Impl {
	GLuint id;

	~Impl();
};
#endif

} // namespace jngl
