// Copyright 2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fixture.hpp"

#include <boost/test/unit_test.hpp>
#include <jngl/shapes.hpp>
#include <jngl/matrix.hpp>

BOOST_AUTO_TEST_CASE(shapes) {
	Fixture f(2);
	auto mv = jngl::modelview();
	mv.translate({ -150, -30 });
	jngl::drawRect(mv, { 120, 40 }, jngl::Color(150, 150, 150));
	mv.translate({ 100, 10 });
	jngl::setColor(255, 255, 255); // should be ignored
	jngl::drawRect(mv, { 120, 40 }, jngl::Color(80, 80, 80));
	mv.translate({ 100, 10 });
	jngl::drawRect(mv, { 120, 40 }, jngl::Color(0, 0, 0));
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒▒▒▒▒▒▒    ░░░░░░              ▒
▒░░░░░░    ▓▓▓▓▓▓    ▒▒▒▒▒▒    ▒
▒          ░░░░░░    ██████    ▒
▒                    ▒▒▒▒▒▒    ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}
