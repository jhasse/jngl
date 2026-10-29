// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Fixture.hpp"
#include "TestShaders.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
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

	const jngl::ShaderProgram program = JNGL_TEST_SHADER_PROGRAM(color);
	const int color = program.getUniformLocation("color");
	// Each draw gets its own copy of the uniforms, as they might change in between (which they
	// don't here). With the padding that's 256 bytes, so this is about 500 KiB, the initial buffer
	// has 256 KiB on Vulkan.
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

TEST_CASE("uniform arrays") {
	Fixture f(1.f);
	const std::vector<uint8_t> white(40 * 20 * 4, 255);
	const jngl::Sprite sprite(white.data(), 40, 20);
	const jngl::Vec2 center{ -40, 0 };
	jngl::drawRect(jngl::modelview().translate(center - sprite.getSize() / 2.), sprite.getSize(),
	               0x000000_rgb);
	const auto expected = f.getAsciiArt();
	Fixture::reset();

	// Its uniforms need more than 256 bytes
	const jngl::ShaderProgram program = JNGL_TEST_SHADER_PROGRAM(colors);
	// Like on OpenGL, the array's name is its first element
	REQUIRE(program.getUniformLocation("colors") == program.getUniformLocation("colors[0]"));
	{
		const auto context = program.use();
		jngl::ShaderProgram::Context::setUniform(program.getUniformLocation("colors"), 1.f, 1.f,
		                                         1.f, 1.f);
		jngl::ShaderProgram::Context::setUniform(program.getUniformLocation("colors[31]"), 0.f, 0.f,
		                                         0.f, 1.f);
		jngl::ShaderProgram::Context::setUniform(program.getUniformLocation("index"), 31);
		sprite.draw(jngl::modelview().translate(center), &program);
	}
	REQUIRE(f.getAsciiArt() == expected);
}

TEST_CASE("Sprite::batch with a custom ShaderProgram") {
	Fixture f(1.f);
	const std::vector<uint8_t> white(40 * 20 * 4, 255);
	const jngl::Sprite sprite(white.data(), 40, 20);
	const jngl::Vec2 center{ -40, 0 };
	// So that there's something to compare in either case
	const auto drawReference = []() {
		jngl::drawRect(jngl::modelview().translate({ 60, -10 }), { 20, 20 }, 0x000000_rgb);
	};
	drawReference();
	jngl::drawRect(jngl::modelview().translate(center - sprite.getSize() / 2.), sprite.getSize(),
	               0x000000_rgb);
	const auto expected = f.getAsciiArt();
	Fixture::reset();

	drawReference();
	// Draws it black, while it'd be invisible on the white window otherwise
	const jngl::ShaderProgram program = JNGL_TEST_SHADER_PROGRAM(black);
	{
		const auto batch = sprite.batch(&program);
		batch.draw(jngl::modelview().translate(center));
	}
	REQUIRE(f.getAsciiArt() == expected);
}
