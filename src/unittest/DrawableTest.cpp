// Copyright 2019-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Drawable.hpp"

#include <boost/test/unit_test.hpp>

class Dummy : public jngl::Drawable {
public:
	void step() override {
	}
	void draw() const override {
	}
};

BOOST_AUTO_TEST_CASE(Drawable) {
	Dummy d;
	BOOST_CHECK_CLOSE(d.getX(), 0, 1e-9);
	BOOST_CHECK_CLOSE(d.getY(), 0, 1e-9);
	BOOST_CHECK_CLOSE(d.getWidth(), 0, 1e-9);
	BOOST_CHECK_CLOSE(d.getHeight(), 0, 1e-9);
	BOOST_CHECK_CLOSE(d.getWidth(), d.getSize().x, 1e-9);
	BOOST_CHECK_CLOSE(d.getHeight(), d.getSize().y, 1e-9);
}
