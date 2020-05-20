// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/framebuffer.hpp"
#include "../jngl/sprite.hpp"
#include "../jngl/text.hpp"
#include "Fixture.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(FrameBuffer) {
	Fixture f(1.f);
	jngl::FrameBuffer fb(320, 70);
	jngl::Text text("foobar");
	jngl::Text background("background");
	background.setLeft(-10);
	background.draw();
	{
		const auto context = fb.use();
		text.draw();
	}
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒░                             ▒
▓░░░░░░                        ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	background.draw();
	fb.draw(-160, -35);
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒░              ░              ▒
▓░░░░░░         ░░░░           ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	jngl::setSpriteAlpha(150);
	fb.draw(-160, -35);
	jngl::setSpriteAlpha(255);
	background.draw();
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒░                             ▒
▓░░░░░░           ░            ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}
