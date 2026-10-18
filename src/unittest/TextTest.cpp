// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <jngl/font.hpp>
#include <jngl/text.hpp>
#include <string_view>

TEST_CASE("CharacterTest") {
	Fixture f(2);
	jngl::Font font("../data/Arial.ttf", 40);
	jngl::Text t("m ö o ß");
	t.setFont(font);
	t.setPos(-110, -20);
	t.draw();
	const auto output = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒           ░       ░▒         ▒
▒    ░░░░  ░▒   ░░  ▒░░        ▒
▒    ▒░▒▒  ▒ ▒ ░░▒  ▒▒         ▒
▒    ▒ ▒░  ▒ ▒ ░░▒  ▒ ▒        ▒
▒    ░ ░░  ░▒░  ▒░  ░░░        ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
	REQUIRE(f.getAsciiArt() == output);
	font.print("m ö o ß", -110, -20);
	REQUIRE(f.getAsciiArt() == output);
	font.print("m ö o ß", jngl::Vec2(-110, -20));
	REQUIRE(f.getAsciiArt() == output);
};

TEST_CASE("TextTest") {
	// Test with two rather big scale factors to avoid rounding errors:
	for (double scaleFactor : { 6, 8 }) {
		Fixture f(scaleFactor);
		jngl::setFont("../data/Arial.ttf");
		jngl::Text text("test string\nline 2");
		REQUIRE_THAT(text.getWidth(), Catch::Matchers::WithinAbs(69.4, 0.1));
		REQUIRE(std::lround(text.getHeight()) == 38);
		text.setCenter(-10, -10);
		REQUIRE_THAT(text.getX(), Catch::Matchers::WithinAbs(-44.7, 0.1));
		text.draw();
		const std::string screenshotCentered = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒          ░░▒░░░░░            ▒
▒                              ▒
▒           ░░                 ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
		REQUIRE(f.getAsciiArt() == screenshotCentered);

		text.setPos(-10, -10);
		REQUIRE(std::lround(text.getX()) == -10);
		text.draw();
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                  ░           ▒
▒              ░░░░░░░         ▒
▒              ░  ░            ▒
▒              ░░ ░            ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

		text.setCenter(-10, -10); // restore previous state
		text.draw();
		REQUIRE_THAT(text.getX(), Catch::Matchers::WithinAbs(-44.7, 0.1));
		REQUIRE(f.getAsciiArt() == screenshotCentered);

		text.setAlign(jngl::Alignment::CENTER);
		REQUIRE_THAT(text.getX(), Catch::Matchers::WithinAbs(-44.7, 0.1));
		text.draw(); // the second line should now be centered below the first
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒          ░░▒░░░░░            ▒
▒               ░              ▒
▒            ░░░░              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

		REQUIRE(std::lround(jngl::getTextWidth("foo")) == 22);
		REQUIRE(std::lround(jngl::getTextWidth("foo\nfoobar\nbar")) == 45);
	}
}
