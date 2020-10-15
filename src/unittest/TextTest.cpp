// Copyright 2018-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/text.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(CharacterTest) {
	Fixture f(1);
	jngl::Font font("../data/Arial.ttf", 40);
	jngl::Text t("m ö o ß");
	t.setFont(font);
	t.setPos(-110, -20);
	t.draw();
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒           ░        ░         ▒
▒     ░ ░  ░░   ░   ▒░░        ▒
▒    ▒░▒▒  ▒░▒ ░░▒  ▒▒░        ▒
▒    ▒ ▒░  ▒ ▒ ░░▒  ▒ ▒        ▒
▒    ░ ░░  ░▒   ▒░  ░░░        ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}

BOOST_AUTO_TEST_CASE(TextTest) {
	// Test with two rather big scale factors to avoid rounding errors:
	for (double scaleFactor : { 6, 8 }) {
		Fixture f(scaleFactor);
		jngl::setFont("../data/Arial.ttf");
		jngl::Text text("test string\nline 2");
		BOOST_CHECK_EQUAL(std::lround(text.getWidth()), 69L);
		BOOST_CHECK_EQUAL(std::lround(text.getHeight()), 38L);
		text.setCenter(-10, -10);
		BOOST_CHECK_CLOSE(text.getX(), -44.5, 1e-9);
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
		BOOST_CHECK_EQUAL(f.getAsciiArt(), screenshotCentered);

		text.setPos(-10, -10);
		BOOST_CHECK_EQUAL(std::lround(text.getX()), -10L);
		text.draw();
		BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
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
		BOOST_CHECK_CLOSE(text.getX(), -44.5, 1e-9);
		BOOST_CHECK_EQUAL(f.getAsciiArt(), screenshotCentered);

		text.setAlign(jngl::Alignment::CENTER);
		BOOST_CHECK_CLOSE(text.getX(), -44.5, 1e-9);
		text.draw(); // the second line should now be centered below the first
		BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒          ░░▒░░░░░            ▒
▒               ░              ▒
▒            ░░░░              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
}
