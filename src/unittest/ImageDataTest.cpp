// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/ImageData.hpp"
#include "Fixture.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(ImageData) {
	try {
		jngl::ImageData::load("foo.tga");
		BOOST_ASSERT(false);
	} catch (std::runtime_error& e) {
		BOOST_CHECK_EQUAL(std::string(e.what()).substr(0, 68),
		                  "No suitable image file found for: foo.tga\nSupported file extensions:");
	}
	try {
		jngl::ImageData::load("foo.webp");
		BOOST_ASSERT(false);
	} catch (std::runtime_error& e) {
		BOOST_CHECK_EQUAL(e.what(), "File not found: foo.webp");
	}
}
