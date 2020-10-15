// Copyright 2018-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/sprite.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/qvm/vec_operations.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(Sprite) {
	for (float factor : { 1.f, 2.f, 3.4f }) {
		Fixture f(factor);
		jngl::drawScaled("../data/jngl.webp", -60, -30, 0.2f);
		BOOST_CHECK_EQUAL(f.getAsciiArt(), R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
		jngl::load("../data/jngl.webp"); // This shouldn't crash
	}
	{
		Fixture f(3.4f);
		jngl::Sprite sprite("../data/jngl.webp");
		BOOST_CHECK_CLOSE(sprite.getWidth(), 600, 1e-9);
		BOOST_CHECK_CLOSE(sprite.getHeight(), 300, 1e-9);

		BOOST_CHECK_CLOSE(sprite.getSize().x, 600, 1e-5); // rounding errors are strong in this one
		BOOST_CHECK_CLOSE(sprite.getSize().y, 300, 1e-5);

		sprite.setCenter(7.3f, 9.1f);
		BOOST_CHECK_CLOSE(boost::qvm::mag(sprite.getCenter<jngl::Vec2>() - jngl::Vec2(7.3f, 9.1f)),
		                  0, 1e-5);
		BOOST_CHECK_CLOSE(sprite.getX(), -292.7f, 1e-5);
		BOOST_CHECK_CLOSE(sprite.getY(), -140.9f, 1e-5);
		BOOST_CHECK_CLOSE(sprite.getLeft(), -132.7f, 1e-5);
		BOOST_CHECK_CLOSE(sprite.getTop(), -105.9f, 1e-5);
		BOOST_CHECK_CLOSE(sprite.getRight(), -147.3f, 1e-5);
		BOOST_CHECK_CLOSE(sprite.getBottom(), -124.1f, 1e-5);
	}
}
