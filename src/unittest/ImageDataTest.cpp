// Copyright 2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/ImageData.hpp"
#include "Fixture.hpp"

#include <boost/ut.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut;
	"ImageData"_test = [] {
		try {
			jngl::ImageData::load("foo.tga");
			expect(false);
		} catch (std::runtime_error& e) {
			expect(
			    eq(std::string(e.what()).substr(0, 68),
			       std::string(
			           "No suitable image file found for: foo.tga\nSupported file extensions:")));
		}
		try {
			jngl::ImageData::load("foo.webp");
			expect(false);
		} catch (std::runtime_error& e) {
			expect(eq(e.what(), std::string("File not found: foo.webp")));
		}
	};
};
} // namespace
