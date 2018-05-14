// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Finally.hpp"
#include "../jngl/sprite.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(FinallyTest) {
	bool called = false;
	{
		jngl::Finally _([&called]() {
			called = true;
		});
		BOOST_CHECK(!called);
	}
	BOOST_CHECK(called);
}

BOOST_AUTO_TEST_CASE(halfLoadTest) {
	BOOST_CHECK_EQUAL(jngl::getWidth("../jngl"), 600);
	BOOST_CHECK_EQUAL(jngl::getHeight("../jngl"), 300);
	BOOST_CHECK_THROW(jngl::load("../jngl"), std::runtime_error);
}
