// Copyright 2020-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/FrameBuffer.hpp"
#include "../jngl/ScaleablePixels.hpp"
#include "../jngl/Shader.hpp"
#include "../jngl/ShaderProgram.hpp"
#include "../jngl/Sprite.hpp"
#include "../jngl/matrix.hpp"
#include "../jngl/other.hpp"
#include "../jngl/shapes.hpp"
#include "../jngl/window.hpp"
#include "../opengl.hpp"
#include "../window.hpp"
#include "../windowptr.hpp"
#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <vector>

TEST_CASE("FrameBufferTest") {
	Fixture f(1.f);
	jngl::FrameBuffer fb(320_px, 70_px);
	jngl::drawRect(jngl::modelview().translate({ -10, 0 }), { 10, 10 }, 0x000000_rgb);
	{
		const auto context = fb.use();
		jngl::setAlpha(150);
		jngl::drawRect(jngl::modelview().translate({ 10, 0 }), { 30, 10 }, 0x000000_rgb);
		jngl::setAlpha(255);
		jngl::drawRect(jngl::modelview().translate({ 40, 0 }), { 30, 10 }, 0x000000_rgb);
	}
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒               ▒
▒              ▒               ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	jngl::drawRect(jngl::modelview().translate({ -10, 0 }), { 10, 10 }, 0x000000_rgb);
	fb.draw(-160, -35);
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒ ░░░▒▒▒        ▒
▒              ▒ ░░░▒▒▒        ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	jngl::setSpriteAlpha(150);
	fb.draw(-160, -35);
	jngl::setSpriteAlpha(255);
	jngl::drawRect(jngl::modelview().translate({ -10, 0 }), { 10, 10 }, 0x000000_rgb);
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒    ░░░        ▒
▒              ▒    ░░░        ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

	jngl::FrameBuffer fb2(320_px, 70_px);
	{
		const auto context1 = fb.use();
		jngl::drawRect(jngl::modelview().translate({ -40, 0 }), { 10, 10 }, 0x000000_rgb);
		{
			const auto context2 = fb2.use();
			jngl::translate(-10, 0); // FrameBuffer::use() should push/pop the modelview matrix
			jngl::drawRect(jngl::modelview().translate({ 0, -20 }), { 10, 10 }, 0x000000_rgb);
		}
		jngl::drawRect(jngl::modelview().translate({ -80, 0 }), { 10, 10 }, 0x000000_rgb);
	}
	fb2.draw(-160, -35);
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒              ▒               ▒
▒              ▒               ▒
▒                              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

	// Check if drawing on fb *after* fb2 was in use worked
	fb.draw(-160, -35);
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒       ▒   ▒    ░░░▒▒▒        ▒
▒       ▒   ▒    ░░░▒▒▒        ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

	{
		auto context = fb.use();
		context.clear();
		jngl::setAlpha(255);
		jngl::drawCircle(jngl::Vec2{ 0, 0 }, 15);
	}
	jngl::drawCircle(jngl::modelview().translate({ 100, 0 }),
	                 6); // check that the background of the framebuffer is transparent
	fb.draw(-160, -35);
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░██░             ▒
▒             ▒██▒       ▒▒    ▒
▒             ░██░             ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	{
		auto context = fb.use();
		context.clear(0xdd2222_rgb);
		jngl::drawCircle(jngl::Vec2{ 40, 0 }, 15);
	}
	fb.draw(-160, -35);
	REQUIRE(f.getAsciiArt() == R"(
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
)");
};

TEST_CASE("FrameBufferScale") {
	for (float scaleFactor : { 1.f, 5.f }) {
		Fixture f(scaleFactor);
		// check if scaling is correct when using a small FrameBuffer
		jngl::FrameBuffer smallFb(100_sp, 30_sp);
		jngl::drawRect(jngl::modelview().translate({ -10, 0 }), { 10, 10 }, 0x000000_rgb);
		{
			const auto context = smallFb.use();
			jngl::setAlpha(150);
			jngl::drawRect(jngl::modelview().translate({ 10, 0 }), { 30, 10 }, 0x000000_rgb);
			jngl::setAlpha(255);
			jngl::drawRect(jngl::modelview().translate({ 40, 0 }), { 30, 10 }, 0x000000_rgb);
		}
		smallFb.draw(-160, 0);
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒               ▒
▒              ▒               ▒
▒     ░░░█                     ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

		// check drawMesh works with scaling
		{
			auto context = smallFb.use();
			context.clear(0xdd2222_rgb);
			jngl::drawCircle(jngl::Vec2{ 10, 0 }, 15);
		}
		const float width = smallFb.getSize().x;
		const float height = smallFb.getSize().y;
		const float x = -100;
		const float y = -20;
		smallFb.drawMesh({
		    // top right triangle
		    jngl::Vertex{ .x = x, .y = y, .u = 0, .v = 1 },
		    jngl::Vertex{ .x = x + width, .y = y, .u = 1, .v = 1 },
		    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },

		    // bottom left triangle
		    jngl::Vertex{ .x = x, .y = y, .u = 0, .v = 1 },
		    jngl::Vertex{ .x = x, .y = y + height, .u = 0, .v = 0 },
		    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },

		    // top right triangle
		    jngl::Vertex{ .x = x, .y = y + height, .u = 0, .v = 0 },
		    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },
		    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },

		    // bottom left triangle
		    jngl::Vertex{ .x = x, .y = y + height, .u = 0, .v = 0 },
		    jngl::Vertex{ .x = x, .y = y + height, .u = 0, .v = 0 },
		    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },
		});
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒     ░░░░░▒▒░░░               ▒
▒     ▒▒▒▒▓██▓▒▒               ▒
▒     ▒▒▒▒▓██▓▒▒               ▒
▒     ░░░░░▒▒░░░               ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
};

TEST_CASE("FrameBufferDrawMeshModelview") {
	Fixture f(1.f);
	jngl::FrameBuffer fb(320_px, 70_px);
	{
		const auto context = fb.use();
		// Columns 1 to 6 and rows 1 to 2 of the ASCII art
		jngl::drawRect(jngl::modelview().translate({ -150, -25 }), { 60, 20 }, 0x000000_rgb);
	}
	// All of it, with v running from 1 at the top to 0 at the bottom
	const std::vector<jngl::Vertex> all{
		{ .x = -160, .y = -35, .u = 0, .v = 1 }, { .x = 160, .y = -35, .u = 1, .v = 1 },
		{ .x = 160, .y = 35, .u = 1, .v = 0 },   { .x = -160, .y = -35, .u = 0, .v = 1 },
		{ .x = 160, .y = 35, .u = 1, .v = 0 },   { .x = -160, .y = 35, .u = 0, .v = 0 },
	};
	// Moved by the modelview to columns 11 to 16 and rows 3 to 4, and 80% opaque. Neither the
	// global modelview nor the sprite colour should have a say:
	jngl::translate(-100, -100);
	jngl::setSpriteColor(0xff0000_rgb);
	fb.drawMesh(jngl::Mat3().translate({ 100, 20 }), all, 0xffffffcc_rgba);
	jngl::setSpriteColor(0xffffff_rgb);
	jngl::reset();
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒          ▓▓▓▓▓▓              ▒
▒          ▓▓▓▓▓▓              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}

TEST_CASE("FrameBufferInception") {
	// create a FrameBuffer while another FrameBuffer is bound:
	Fixture f{ 1 };
	std::optional<jngl::FrameBuffer> fb1 = jngl::FrameBuffer(30_sp, 30_sp);
	std::optional<jngl::FrameBuffer> fb2;
	{
		const auto context = fb1->use();
		fb2.emplace(30_sp, 30_sp);
		// Still drawing on fb1, not on the window, where this would show in the middle
		jngl::drawRect(jngl::modelview().translate({ -15, -15 }), { 30, 30 }, 0x000000_rgb);
	}
	jngl::drawRect(jngl::modelview().translate({ -150, -25 }), { 10, 10 }, 0x000000_rgb);
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒█                             ▒
▒                              ▒
▒                              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	fb1 = {};
	{
		const auto context = fb2->use();
		jngl::drawRect(jngl::modelview().translate({ 10, 0 }), { 10, 10 }, 0x000000_rgb);
	}
	fb2->draw(0, 0);
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒                              ▒
▒                              ▒
▒                 ▒            ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}

TEST_CASE("FrameBuffer anti-aliased like the window") {
	Fixture f(1.f);
	if (!jngl::pWindow->isMultisampleSupported()) {
		SKIP("Multisampling isn't supported here");
	}
	// A black triangle with a slanted edge, drawn into a FrameBuffer that's then drawn onto the
	// window pixel for pixel, and how many of the pixels around it are grey, i.e. smoothed
	const auto greys = [](const bool antiAliased) {
		Fixture::reset();
		jngl::FrameBuffer fb(320_px, 70_px, false, antiAliased);
		{
			const auto context = fb.use();
			jngl::drawTriangle(jngl::Vec2(-60, -25), jngl::Vec2(60, -25), jngl::Vec2(-60, 25));
		}
		fb.draw(-160, -35);
		const auto pixels = jngl::readPixels();
		int count = 0;
		for (int y = 5; y < 65; ++y) {
			for (int x = 90; x < 230; ++x) {
				const float red = pixels.at(static_cast<size_t>(y * 320 + x) * 3);
				if (red > 0.1f && red < 0.9f) {
					++count;
				}
			}
		}
		return count;
	};
	REQUIRE(greys(false) == 0); // stair-stepped, as a FrameBuffer always was
	REQUIRE(greys(true) > 50); // about one along each of the edge's 120 columns
#ifdef GL_MULTISAMPLE_ARB
	jngl::setAntiAliasing(false); // also for one that's anti-aliased otherwise
	REQUIRE(greys(true) == 0);
	jngl::setAntiAliasing(true);
#endif
}

TEST_CASE("Window still anti-aliased after a FrameBuffer has been used") {
	Fixture f(1.f);
	if (!jngl::pWindow->isMultisampleSupported()) {
		SKIP("Multisampling isn't supported here");
	}
	// How many pixels around a black triangle with a slanted edge are grey, i.e. smoothed
	const auto greys = []() {
		jngl::drawTriangle(jngl::Vec2(-60, -25), jngl::Vec2(60, -25), jngl::Vec2(-60, 25));
		const auto pixels = jngl::readPixels();
		int count = 0;
		for (int y = 5; y < 65; ++y) {
			for (int x = 90; x < 230; ++x) {
				const float red = pixels.at(static_cast<size_t>(y * 320 + x) * 3);
				if (red > 0.1f && red < 0.9f) {
					++count;
				}
			}
		}
		Fixture::reset();
		return count;
	};
	REQUIRE(greys() > 50); // about one along each of the edge's 120 columns
	jngl::FrameBuffer fb(10_px, 10_px);
	{
		const auto context = fb.use();
	}
	REQUIRE(greys() > 50);
	// Reading the window's pixels in the middle of a frame doesn't stop it, either
	(void)jngl::readPixels();
	REQUIRE(greys() > 50);
}

TEST_CASE("Window keeps what's been drawn before a FrameBuffer is drawn into") {
	Fixture f(1.f);
	if (!jngl::pWindow->isMultisampleSupported()) {
		SKIP("Multisampling isn't supported here");
	}
	// A black triangle with a slanted edge on the left, then a FrameBuffer, then a black square on
	// the right: both on the window, the triangle smoothed
	jngl::drawTriangle(jngl::Vec2(-150, -25), jngl::Vec2(-30, -25), jngl::Vec2(-150, 25));
	jngl::FrameBuffer fb(10_px, 10_px);
	{
		const auto context = fb.use();
		jngl::drawRect(jngl::modelview().translate({ -5, -5 }), { 10, 10 }, 0x000000_rgb);
	}
	jngl::drawRect(jngl::modelview().translate({ 40, -20 }), { 40, 40 }, 0x000000_rgb);
	const auto pixels = jngl::readPixels();
	const auto red = [&pixels](const int x, const int y) {
		return pixels.at(static_cast<size_t>(y * 320 + x) * 3);
	};
	int greys = 0;
	for (int y = 5; y < 65; ++y) {
		for (int x = 5; x < 135; ++x) {
			if (red(x, y) > 0.1f && red(x, y) < 0.9f) {
				++greys;
			}
		}
	}
	CHECK(greys > 50); // about one along each of the edge's 120 columns
	CHECK(red(20, 35) < 0.1f); // in the triangle
	CHECK(red(220, 35) < 0.1f); // in the square
	CHECK(red(160, 35) > 0.9f); // between them
}

TEST_CASE("FrameBuffer anti-aliased keeps what's been drawn before another is drawn into") {
	Fixture f(1.f);
	if (!jngl::pWindow->isMultisampleSupported()) {
		SKIP("Multisampling isn't supported here");
	}
	// Like the window above, but in an anti-aliased FrameBuffer, which is then drawn onto the
	// window pixel for pixel
	jngl::FrameBuffer fb(320_px, 70_px, false, true);
	{
		const auto context = fb.use();
		jngl::drawTriangle(jngl::Vec2(-150, -25), jngl::Vec2(-30, -25), jngl::Vec2(-150, 25));
		jngl::FrameBuffer inner(10_px, 10_px);
		{
			const auto innerContext = inner.use();
			jngl::drawRect(jngl::modelview().translate({ -5, -5 }), { 10, 10 }, 0x000000_rgb);
		}
		jngl::drawRect(jngl::modelview().translate({ 40, -20 }), { 40, 40 }, 0x000000_rgb);
	}
	fb.draw(-160, -35);
	const auto pixels = jngl::readPixels();
	const auto red = [&pixels](const int x, const int y) {
		return pixels.at(static_cast<size_t>(y * 320 + x) * 3);
	};
	int greys = 0;
	for (int y = 5; y < 65; ++y) {
		for (int x = 5; x < 135; ++x) {
			if (red(x, y) > 0.1f && red(x, y) < 0.9f) {
				++greys;
			}
		}
	}
	CHECK(greys > 50); // about one along each of the edge's 120 columns
	CHECK(red(20, 35) < 0.1f); // in the triangle
	CHECK(red(220, 35) < 0.1f); // in the square
	CHECK(red(160, 35) > 0.9f); // between them
}

TEST_CASE("FrameBuffer anti-aliased keeps what's drawn while anti-aliasing is off") {
	Fixture f(1.f);
	if (!jngl::pWindow->isMultisampleSupported()) {
		SKIP("Multisampling isn't supported here");
	}
	jngl::FrameBuffer fb(320_px, 70_px, false, true);
	jngl::setAntiAliasing(false);
	{
		const auto context = fb.use();
		jngl::drawRect(jngl::modelview().translate({ -100, -25 }), { 40, 30 }, 0x000000_rgb);
	}
	jngl::setAntiAliasing(true);
	{
		const auto context = fb.use();
		jngl::drawRect(jngl::modelview().translate({ 60, -25 }), { 40, 30 }, 0x000000_rgb);
	}
	// The left one's still there after the FrameBuffer's been drawn into multisampled again
	fb.draw(-160, -35);
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒     ████            ████     ▒
▒     ████            ████     ▒
▒     ████            ████     ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}

TEST_CASE("HDR FrameBuffer keeps values above 1") {
	Fixture f(1.f);
	const std::vector<uint8_t> opaque(40 * 20 * 4, 255);
	const jngl::Sprite sprite(opaque.data(), 40, 20);
	const jngl::Shader two(R"(#version 300 es
		out mediump vec4 outColor;

		void main() {
			outColor = vec4(2.0, 2.0, 2.0, 1.0);
		})",
	                       jngl::Shader::Type::FRAGMENT);
	const jngl::ShaderProgram drawTwo(jngl::Sprite::vertexShader(), two);
	const jngl::Shader half(R"(#version 300 es
		uniform sampler2D tex;

		in mediump vec2 texCoord;

		out mediump vec4 outColor;

		void main() {
			mediump vec4 color = texture(tex, texCoord);
			outColor = vec4(color.rgb * 0.5, color.a);
		})",
	                        jngl::Shader::Type::FRAGMENT);
	const jngl::ShaderProgram drawHalf(jngl::Sprite::vertexShader(), half);
	// Draws 2 into the middle of a FrameBuffer, and the FrameBuffer halved onto the window, where
	// this returns the middle's red as 0 to 255
	const auto drawn = [&](const bool hdr, const bool antiAliased) {
		Fixture::reset();
		jngl::FrameBuffer fb(320_px, 70_px, hdr, antiAliased);
		{
			const auto context = fb.use();
			sprite.draw(jngl::modelview(), &drawTwo);
		}
		fb.draw(jngl::Vec2(-160, -35), &drawHalf);
		const auto pixels = jngl::readPixels();
		return std::lround(pixels.at(static_cast<size_t>(35 * 320 + 160) * 3) * 255);
	};
	const auto clamped = drawn(false, false); // to 1, which is halved
	REQUIRE((clamped > 120 && clamped < 135));
	REQUIRE(drawn(true, false) == 255);
	REQUIRE(drawn(true, true) == 255);
}

TEST_CASE("FrameBuffer adds up alpha unless a BlendMode is set") {
	Fixture f(1.f);
	// Two half transparent black squares on top of each other in a FrameBuffer that's then drawn
	// onto the white window, and how bright they are there, from 0 to 255
	const auto brightness = [](const std::optional<jngl::BlendMode> mode) {
		Fixture::reset();
		jngl::FrameBuffer fb(320_px, 70_px);
		{
			const auto context = fb.use();
			std::optional<jngl::Finally> blendMode;
			if (mode) {
				blendMode.emplace(jngl::setBlendMode(*mode));
			}
			for (int i = 0; i < 2; ++i) {
				jngl::drawRect(jngl::modelview().translate({ -10, -10 }), { 20, 20 },
				               jngl::Rgba(0, 0, 0, 0.5f));
			}
		}
		fb.draw(-160, -35);
		const auto pixels = jngl::readPixels();
		return std::lround(pixels.at(static_cast<size_t>(35 * 320 + 160) * 3) * 255);
	};
	// A quarter as bright as white, like when drawn directly onto the window, as the FrameBuffer
	// ends up opaque there
	REQUIRE(std::abs(brightness(std::nullopt) - 64) <= 3);
	// Composited, they cover three quarters of it, which lets the window shine through
	REQUIRE(std::abs(brightness(jngl::BlendMode::Composite) - 112) <= 3);
}
