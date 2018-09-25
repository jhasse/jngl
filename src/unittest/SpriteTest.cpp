// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/sprite.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(SpriteTest) {
	Fixture f(1);
	jngl::drawScaled("../jngl.webp", -60, -30, 0.2f);
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	jngl::load("../jngl.webp"); // This shouldn't crash
}
