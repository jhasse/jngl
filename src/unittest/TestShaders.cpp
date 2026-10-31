// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "TestShaders.hpp"

#ifndef JNGL_D3D12
#include <jngl/Shader.hpp>
#include <jngl/Sprite.hpp>
#include <jngl/other.hpp>
#include <stdexcept>
#include <string>

namespace {
jngl::Shader readShader(const std::string& path, const jngl::Shader::Type type) {
	const auto source = jngl::readAsset(path);
	if (!source) {
		throw std::runtime_error("Couldn't read " + path);
	}
	return { source, type };
}
} // namespace

jngl::ShaderProgram testShaderProgram(const char* const name, const bool vertexShader) {
	const std::string path = std::string("unittest/") + name;
	const jngl::Shader fragment = readShader(path + ".frag", jngl::Shader::Type::FRAGMENT);
	if (vertexShader) {
		return { readShader(path + ".vert", jngl::Shader::Type::VERTEX), fragment };
	}
	return { jngl::Sprite::vertexShader(), fragment };
}
#endif
