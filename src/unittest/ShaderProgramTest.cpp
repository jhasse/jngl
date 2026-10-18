// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <jngl/Shader.hpp>
#include <jngl/ShaderProgram.hpp>
#include <jngl/Sprite.hpp>
#include <jngl/matrix.hpp>
#include <jngl/shapes.hpp>
#include <vector>

TEST_CASE("more custom uniforms in a frame than fit into the Vulkan backend's initial buffer") {
	Fixture f(1.f);
	const std::vector<uint8_t> opaque(40 * 20 * 4, 255);
	const jngl::Sprite sprite(opaque.data(), 40, 20);
	const jngl::Vec2 center{ -40, 0 };
	jngl::drawRect(jngl::modelview().translate(center - sprite.getSize() / 2.), sprite.getSize(),
	               0x000000_rgb);
	const auto expected = f.getAsciiArt();
	Fixture::reset();

	const jngl::Shader fragment(R"(#version 300 es
		uniform sampler2D tex;
		uniform lowp vec4 color;

		in mediump vec2 texCoord;

		out lowp vec4 outColor;

		void main() {
			outColor = vec4(color.rgb, texture(tex, texCoord).a);
		})",
	                            jngl::Shader::Type::FRAGMENT);
	const jngl::ShaderProgram program(jngl::Sprite::vertexShader(), fragment);
	const int color = program.getUniformLocation("color");
	// Each draw gets its own copy of the uniforms, as they might change in between (which they
	// don't here). On Vulkan that's 256 bytes, so this is about 500 KiB, the initial buffer has
	// 256 KiB.
	for (int i = 0; i < 2000; ++i) {
		const auto context = program.use();
		jngl::ShaderProgram::Context::setUniform(color, 1.f, 1.f, 1.f, 1.f);
		sprite.draw(jngl::modelview().translate(center), &program);
	}
	{
		const auto context = program.use();
		jngl::ShaderProgram::Context::setUniform(color, 0.f, 0.f, 0.f, 1.f);
		sprite.draw(jngl::modelview().translate(center), &program);
	}
	REQUIRE(f.getAsciiArt() == expected);
}
