// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/framebuffer.hpp"
#include "../jngl/shapes.hpp"
#include "../jngl/sprite.hpp"
#include "Fixture.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(FrameBuffer) {
	Fixture f(1.f);
	jngl::FrameBuffer fb(320, 70);
	jngl::drawRect({-10, 0}, {10, 10});
	{
		const auto context = fb.use();
		jngl::setAlpha(150);
		jngl::drawRect({10, 0}, {30, 10});
		jngl::setAlpha(255);
		jngl::drawRect({40, 0}, {30, 10});
	}
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒               ▒
▒              ▒               ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	jngl::drawRect({-10, 0}, {10, 10});
	fb.draw(-160, -35);
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
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
	jngl::drawRect({-10, 0}, {10, 10});
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒    ░░░        ▒
▒              ▒    ░░░        ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

	jngl::FrameBuffer fb2(320, 70);
	{
		const auto context1 = fb.use();
		jngl::drawRect({-40, 0}, {10, 10});
		{
			const auto context2 = fb2.use();
			jngl::drawRect({-10, -20}, {10, 10});
		}
		jngl::drawRect({-80, 0}, {10, 10});
	}
	fb2.draw(-160, -35);
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
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
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
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
		jngl::drawCircle({0, 0}, 15);
	}
	fb.draw(-160, -35);
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░██░             ▒
▒             ▒██▒             ▒
▒             ░██░             ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	{
		auto context = fb.use();
		context.clear(0xdd2222_rgb);
		jngl::drawCircle({40, 0}, 15);
	}
	fb.draw(-160, -35);
	BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
)");
}
