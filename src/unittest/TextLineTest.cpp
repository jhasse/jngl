// Copyright 2022-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/TextLine.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>

TEST_CASE("TextLineTest") { // NOLINT
	// Test with two rather big scale factors to avoid rounding errors:
	for (double scaleFactor : { 6, 8 }) {
		Fixture f(scaleFactor);
		jngl::Font font("../data/Arial.ttf", 14);
		jngl::TextLine text(font, "test string");
		REQUIRE_THAT(text.getWidth(), Catch::Matchers::WithinAbs(80.7, 0.2));
		REQUIRE(std::lround(text.getHeight()) == 22);
		text.setCenter(-80, -10);
		REQUIRE_THAT(text.getX(), Catch::Matchers::WithinAbs(-120.35, 0.2));
		text.draw();
		const std::string screenshotCentered = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒   ▒░░░░░░░                   ▒
▒          ░                   ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
		REQUIRE(f.getAsciiArt() == screenshotCentered);
		text.draw(jngl::modelview());
		REQUIRE(f.getAsciiArt() == screenshotCentered);
	}
}
