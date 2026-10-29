// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "TestShaders.hpp"

#ifndef JNGL_D3D12
#include <jngl/Shader.hpp>
#include <jngl/Sprite.hpp>
#include <jngl/other.hpp>
#include <stdexcept>
#include <string>

jngl::ShaderProgram testShaderProgram(const char* const name) {
	const std::string path = std::string("unittest/") + name + ".frag";
	const auto source = jngl::readAsset(path);
	if (!source) {
		throw std::runtime_error("Couldn't read " + path);
	}
	return { jngl::Sprite::vertexShader(), jngl::Shader(source, jngl::Shader::Type::FRAGMENT) };
}
#endif
