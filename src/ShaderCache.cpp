// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "ShaderCache.hpp"

#include "BuiltinShaders.hpp"
#include "jngl/Alpha.hpp"
#include "jngl/Shader.hpp"
#include "opengl.hpp"
#include "spriteimpl.hpp"

#include <cassert>

namespace jngl {

ShaderCache::ShaderCache() {
	using namespace builtin_shaders;
	const Shader vertexShader(coloredVertex, Shader::Type::VERTEX, coloredVertexGles2);
	const Shader fragmentShader(coloredFragment, Shader::Type::FRAGMENT, coloredFragmentGles2);
	simpleShaderProgram = std::make_unique<ShaderProgram>(vertexShader, fragmentShader);
	simpleModelviewUniform = simpleShaderProgram->getUniformLocation("modelview");
	simpleColorUniform = simpleShaderProgram->getUniformLocation("color");

	{
		const Shader rrVertexShader(roundedVertex, Shader::Type::VERTEX, roundedVertexGles2);
		const Shader rrFragmentShader(roundedFragment, Shader::Type::FRAGMENT,
		                              roundedFragmentGles2);
		roundedRectShaderProgram =
		    std::make_unique<ShaderProgram>(rrVertexShader, rrFragmentShader);
		roundedRectModelviewUniform = roundedRectShaderProgram->getUniformLocation("modelview");
		roundedRectColorUniform = roundedRectShaderProgram->getUniformLocation("color");
		roundedRectSizeUniform = roundedRectShaderProgram->getUniformLocation("size");
		roundedRectCornerRadiiUniform = roundedRectShaderProgram->getUniformLocation("cornerRadii");
	}

	{
		textureVertexShader =
		    std::make_unique<Shader>(texturedVertex, Shader::Type::VERTEX, texturedVertexGles2);
		const Shader fragmentShader(texturedFragment, Shader::Type::FRAGMENT,
		                            texturedFragmentGles2);
		textureShaderProgram =
		    std::make_unique<ShaderProgram>(*textureVertexShader, fragmentShader);
		shaderLerpColorUniform = textureShaderProgram->getUniformLocation("lerpColor");
		shaderLerpFactorsUniform = textureShaderProgram->getUniformLocation("lerpFactors");
		modelviewUniform = textureShaderProgram->getUniformLocation("modelview");
	}
}

ShaderCache::~ShaderCache() = default;

ShaderProgram::Context ShaderCache::useSimpleShaderProgram() {
	return useSimpleShaderProgram(opengl::modelview, gShapeColor);
}

ShaderProgram::Context ShaderCache::useSimpleShaderProgram(const Mat3& modelview, Rgba color) {
	auto context = simpleShaderProgram->use();
	glUniform4f(simpleColorUniform, color.getRed(), color.getGreen(), color.getBlue(),
	            color.getAlpha());
	glUniformMatrix3fv(simpleModelviewUniform, 1, GL_FALSE, modelview.data);

	assert(simpleShaderProgram->getAttribLocation("position") == 0);
	glEnableVertexAttribArray(0);

	return context;
}

void ShaderCache::setTextureColorUniforms(const Rgba color) const {
	// Multiplying with color is the same as mixing towards black by 1 - color
	glUniform3f(shaderLerpColorUniform, 0, 0, 0);
	glUniform4f(shaderLerpFactorsUniform, 1.f - color.getRed(), 1.f - color.getGreen(),
	            1.f - color.getBlue(), 1.f - color.getAlpha());
}

void ShaderCache::setTextureLerpUniforms(const Rgb color, const float red, const float green,
                                         const float blue, const Alpha alpha) const {
	glUniform3f(shaderLerpColorUniform, color.getRed(), color.getGreen(), color.getBlue());
	glUniform4f(shaderLerpFactorsUniform, red, green, blue, 1.f - alpha.getAlpha());
}

void ShaderCache::drawTriangle(const Vec2 a, const Vec2 b, const Vec2 c) {
	opengl::bindVertexArray(opengl::vaoStream);
	auto tmp = useSimpleShaderProgram();
	const float vertexes[] = { static_cast<float>(a.x), static_cast<float>(a.y),
		                       static_cast<float>(b.x), static_cast<float>(b.y),
		                       static_cast<float>(c.x), static_cast<float>(c.y) };
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
	// STREAM because we're using the buffer only once
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertexes, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void ShaderCache::drawTriangle(const Mat3& modelview, Rgba color) {
	opengl::bindVertexArray(opengl::vaoStream);
	auto tmp = useSimpleShaderProgram(modelview, color);
	const float vertexes[] = {
		0, -1, -std::sqrt(3.f) / 2, 0.5, std::sqrt(3.f) / 2, 0.5,
	};
	glBindBuffer(GL_ARRAY_BUFFER, opengl::vboStream); // VAO does NOT save the VBO binding
	// STREAM because we're using the buffer only once
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertexes, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

ShaderProgram::Context ShaderCache::useRoundedRectShaderProgram(const Mat3& modelview, Rgba color,
                                                                Vec2 size, float topLeft,
                                                                float topRight, float bottomLeft,
                                                                float bottomRight) {
	auto context = roundedRectShaderProgram->use();
	glUniform4f(roundedRectColorUniform, color.getRed(), color.getGreen(), color.getBlue(),
	            color.getAlpha());
	glUniformMatrix3fv(roundedRectModelviewUniform, 1, GL_FALSE, modelview.data);
	glUniform2f(roundedRectSizeUniform, static_cast<float>(size.x), static_cast<float>(size.y));
	glUniform4f(roundedRectCornerRadiiUniform, topLeft, topRight, bottomLeft, bottomRight);

	assert(roundedRectShaderProgram->getAttribLocation("position") == 0);
	glEnableVertexAttribArray(0);

	return context;
}

} // namespace jngl
