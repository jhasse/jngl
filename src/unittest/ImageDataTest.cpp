// Copyright 2023-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/ImageData.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ImageData") {
	try {
		jngl::ImageData::load("foo.tga");
		REQUIRE(false);
	} catch (std::runtime_error& e) {
		REQUIRE(std::string(e.what()).substr(0, 68) ==
		        "No suitable image file found for: foo.tga\nSupported file extensions:");
	}
	try {
		jngl::ImageData::load("foo.webp");
		REQUIRE(false);
	} catch (std::runtime_error& e) {
		REQUIRE(std::string(e.what()) == "File not found: foo.webp");
	}
	REQUIRE(jngl::ImageData::load("../data/jngl.webp")->getImageWidth() == 600);
	REQUIRE(jngl::ImageData::load("../data/jngl")->getImageHeight() == 300);
}
