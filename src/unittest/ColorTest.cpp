// Copyright 2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Color.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(Color) {
	const auto a = 0x68da4f_rgb;
	const auto b = jngl::Color(0x68, 0xda, 0x4f);
	const auto c = jngl::Color(104, 218, 79);
	BOOST_CHECK_EQUAL(a.getRed(), b.getRed());
	BOOST_CHECK_EQUAL(c.getRed(), b.getRed());
	BOOST_CHECK_EQUAL(a.getGreen(), b.getGreen());
	BOOST_CHECK_EQUAL(c.getGreen(), b.getGreen());
	BOOST_CHECK_EQUAL(a.getBlue(), b.getBlue());
	BOOST_CHECK_EQUAL(c.getBlue(), b.getBlue());
}
