// Copyright 2020-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/FrameBuffer.hpp"
#include "../jngl/ScaleablePixels.hpp"
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
