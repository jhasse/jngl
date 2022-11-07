// Copyright 2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/TextLine.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(TextLineTest) {
	// Test with two rather big scale factors to avoid rounding errors:
	for (double scaleFactor : { 6, 8 }) {
		Fixture f(scaleFactor);
		jngl::Font font("../data/Arial.ttf", 14);
		jngl::TextLine text(font, "test string");
		BOOST_CHECK_CLOSE(text.getWidth(), 80.7, 0.2);
		BOOST_CHECK_EQUAL(std::lround(text.getHeight()), 22L);
		text.setCenter(-80, -10);
		BOOST_CHECK_CLOSE(text.getX(), -120.35, 0.2);
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
		BOOST_CHECK_EQUAL(f.getAsciiArt(), screenshotCentered);
		text.draw(jngl::modelview());
		BOOST_CHECK_EQUAL(f.getAsciiArt(), screenshotCentered);
	}
}
