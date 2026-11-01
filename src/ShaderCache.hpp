// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/Rgba.hpp"
#include "jngl/ShaderProgram.hpp"
#include "jngl/Singleton.hpp"
#include "jngl/Vec2.hpp"

#include <array>
#include <optional>

namespace jngl {

class Alpha;
class Mat3;

class ShaderCache : public Singleton<ShaderCache> {
public:
	ShaderCache();
	~ShaderCache();

	ShaderProgram::Context useSimpleShaderProgram();
	ShaderProgram::Context useSimpleShaderProgram(const Mat3& modelview, Rgba color);

	std::unique_ptr<Shader> textureVertexShader;
	std::unique_ptr<ShaderProgram> textureShaderProgram;
	int modelviewUniform;

	/// Sets the color uniforms of textureShaderProgram, which has to be in use, so that the
	/// texture gets multiplied with \a color
	void setTextureColorUniforms(Rgba color) const;

	/// Sets the color uniforms of textureShaderProgram, which has to be in use, so that each color
	/// channel of the texture gets linearly interpolated towards \a color
	void setTextureLerpUniforms(Rgb color, float red, float green, float blue, Alpha) const;

	void drawTriangle(Vec2 a, Vec2 b, Vec2 c);
	void drawTriangle(const Mat3& modelview, Rgba color);

	ShaderProgram::Context useRoundedRectShaderProgram(const Mat3& modelview, Rgba color, Vec2 size,
	                                                   float topLeft, float topRight,
	                                                   float bottomLeft, float bottomRight);

private:
	/// lerpColor and lerpFactors of textureShaderProgram, unless they're set to that already
	void setTextureUniforms(const std::array<float, 7>& lerpColorAndFactors) const;

	int shaderLerpColorUniform;
	int shaderLerpFactorsUniform;
	/// What setTextureUniforms() has set them to, as sprites and characters are often drawn with
	/// the same color one after another
	mutable std::optional<std::array<float, 7>> textureUniforms;

	std::unique_ptr<ShaderProgram> simpleShaderProgram;
	int simpleModelviewUniform;
	int simpleColorUniform;

	std::unique_ptr<ShaderProgram> roundedRectShaderProgram;
	int roundedRectModelviewUniform;
	int roundedRectColorUniform;
	int roundedRectSizeUniform;
	int roundedRectCornerRadiiUniform;
};

} // namespace jngl
