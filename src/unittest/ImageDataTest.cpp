// Copyright 2023-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/ImageData.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE("ImageData") {
	REQUIRE_THROWS_WITH(jngl::ImageData::load("foo.tga"),
	                    Catch::Matchers::StartsWith(
	                        "No suitable image file found for: " JNGL_UNITTEST_DATA "foo.tga\n"
	                        "Supported file extensions:"));
	REQUIRE_THROWS_WITH(jngl::ImageData::load("foo.webp"),
	                    "File not found: " JNGL_UNITTEST_DATA "foo.webp");
	REQUIRE(jngl::ImageData::load("jngl.webp")->getImageWidth() == 600);
	REQUIRE(jngl::ImageData::load("jngl")->getImageHeight() == 300);
}
