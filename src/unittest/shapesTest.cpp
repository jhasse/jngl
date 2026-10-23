// Copyright 2022-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <jngl/Color.hpp>
#include <jngl/Rgba.hpp>
#include <jngl/matrix.hpp>
#include <jngl/shapes.hpp>
#include <jngl/window.hpp>

#include <cmath>
#include <utility>

TEST_CASE("rounded rectangle without corner radii fills the whole rectangle") {
	Fixture f(1);
	const auto modelview = jngl::modelview().translate({ -60, -20 });
	const jngl::Vec2 size{ 120, 40 };
	jngl::drawRect(modelview, size, 0x000000ff_rgba);
	const auto expected = f.getAsciiArt();
	jngl::drawRoundedRect(modelview, size, 0x000000ff_rgba, 0, 0, 0, 0);
	REQUIRE(f.getAsciiArt() == expected);
}

TEST_CASE("shapes") {
	for (double scaleFactor : { 1, 2 }) {
		Fixture f(scaleFactor);
		auto mv = jngl::modelview();
		mv.translate({ -150, -30 });
		jngl::drawRect(mv, { 120, 40 }, jngl::Color(150, 150, 150));
		mv.translate({ 100, 10 });
		jngl::setColor(255, 255, 255); // should be ignored
		jngl::drawRect(mv, { 120, 40 }, jngl::Color(80, 80, 80));
		mv.translate({ 100, 10 });
		jngl::drawRect(mv, { 120, 40 }, jngl::Color(0, 0, 0));
		REQUIRE(f.getAsciiArt() == R"(
▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░        ▒
▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▓
▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓███████████
▒░░░░░░░░░░▓▓▓▓▓▓▓▓▓▓███████████
▒          ░░░░░░░░░░███████████
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒███████████
)");
	}
	for (double scaleFactor : { 1, 2 }) {
		Fixture f(scaleFactor);
		jngl::drawSquare(jngl::modelview().translate({ -100, 0 }).scale(42), 0x111111ff_rgba);
		jngl::drawLine(jngl::modelview().translate({ -20, -20 }), jngl::Vec2(0, 38), 5.f,
		               0x00ff00ff_rgba);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
		jngl::setLineWidth(7);
		jngl::drawLine(jngl::Vec2(15, -20), jngl::Vec2(19, 17));
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
		jngl::drawCircle(jngl::modelview().translate({ 100, -0.1 }).scale(42.05, 11.9),
		                 0xff0000ff_rgba);
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒   ▒▒▒▒         ░             ▒
▒   ████     ░░  ▓     ░▒▒▒▒░  ▒
▒   ████     ░░  ▓    ▓▓▓▓▓▓▓▓ ▒
▒   ████     ░░  ▒░    ░▒▒▒▒░  ▒
▒   ▒▒▒▒                       ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
}
TEST_CASE("circleOutline") {
	for (double scaleFactor : { 1, 2 }) {
		Fixture f(scaleFactor);
		jngl::drawCircleOutline(jngl::modelview().translate({ -80, 0 }), 25, 6, 0x000000ff_rgba);
		jngl::drawCircleOutline(jngl::modelview().translate({ -80, 0 }), 20, 3, "#999999ff"_rgba);
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒     ▓▒▒▓                     ▒
▒    ▒░  ░▒                    ▒
▒    ▓    ▓                    ▒
▒    ▒░  ░▒                    ▒
▒     ▓▒▒▓                     ▒
▓▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
}
TEST_CASE("rectOutline") {
	Fixture f(1.f);
	// Thick and half see-through, from x = 105 to 215 and y = 10 to 60 on the 320x70 window: as
	// grey in its corners, outside and inside, as along its sides, and nothing in the middle
	jngl::drawRectOutline(jngl::modelview(), { 100, 40 }, 10.f, 0x00000080_rgba);
	const auto pixels = jngl::readPixels();
	const auto red = [&](const int x, const int y) {
		return pixels.at(static_cast<size_t>(y * 320 + x) * 3);
	};
	const float side = red(160, 12);
	REQUIRE(side > 0.4f);
	REQUIRE(side < 0.6f);
	for (const auto [x, y] : { std::pair{ 106, 11 }, std::pair{ 213, 58 }, std::pair{ 112, 17 },
	                           std::pair{ 207, 52 }, std::pair{ 107, 35 } }) {
		REQUIRE(std::abs(red(x, y) - side) < 0.02f);
	}
	REQUIRE(red(160, 35) > 0.99f);
}
TEST_CASE("triangle") {
	for (double scaleFactor : { 0.6, 1.5 }) {
		Fixture f(scaleFactor);
		auto mv = jngl::modelview();
		jngl::setColor(0x999999ff_rgba);
		jngl::drawTriangle(jngl::Vec2(-130, 16), jngl::Vec2(-70, 10), jngl::Vec2(-80, -24));
		jngl::drawTriangle(mv.translate({ 50, -4 }).scale(40, 18), 0x000000ff_rgba);
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒      ░            ░░         ▒
▒     ░▒░          ▒██▒        ▒
▒    ░▒▒░         ▓████▓       ▒
▒   ░░░░░                      ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
}
TEST_CASE("ellipse") {
	for (double scaleFactor : { 1, 2 }) {
		Fixture f(scaleFactor);
		auto mv = jngl::modelview();
		// Draw an ellipse centered near the left side
		jngl::drawEllipse(mv.translate({ -120, -27 }), 70, 20);
		REQUIRE(f.getAsciiArt() == R"(
███████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
██████████▒                    ▒
▓▓▓▓▓▓▓▒░                      ▒
▒                              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
}
TEST_CASE("more shapes in a frame than fit into the Vulkan backend's initial vertex buffer") {
	Fixture f(1.f);
	const auto draw = []() {
		jngl::drawRect(jngl::modelview().translate({ -60, -10 }), { 40, 20 }, 0x000000_rgb);
	};
	draw();
	const auto expected = f.getAsciiArt();
	Fixture::reset();
	// Enough rectangles to exceed the Vulkan backend's initial 1 MiB vertex buffer
	for (int i = 0; i < 100'000; ++i) {
		jngl::drawRect(jngl::modelview().translate({ -60, -10 }), { 40, 20 }, 0xffffff_rgb);
	}
	draw();
	REQUIRE(f.getAsciiArt() == expected);
}
