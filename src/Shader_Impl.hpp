// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "jngl/Shader.hpp"
#include "opengl.hpp"

namespace jngl {

struct Shader::Impl {
	GLuint id;
};

} // namespace jngl
