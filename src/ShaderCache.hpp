// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/Rgba.hpp"
#include "jngl/ShaderProgram.hpp"
#include "jngl/Singleton.hpp"

namespace jngl {

class Mat3;

class ShaderCache : public Singleton<ShaderCache> {
public:
	ShaderCache();
	~ShaderCache();

	ShaderProgram::Context useSimpleShaderProgram();
	ShaderProgram::Context useSimpleShaderProgram(const Mat3& modelview, Rgba color);

	std::unique_ptr<Shader> textureVertexShader;
	std::unique_ptr<ShaderProgram> textureShaderProgram;
	int shaderSpriteColorUniform;
	int modelviewUniform;

private:
	std::unique_ptr<ShaderProgram> simpleShaderProgram;
	int simpleModelviewUniform;
	int simpleColorUniform;
};

} // namespace jngl
