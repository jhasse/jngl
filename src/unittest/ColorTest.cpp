// Copyright 2019-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Color.hpp"
#include "../jngl/Rgb.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Color") { // NOLINT
	const jngl::Color a{ 0x68da4f_rgb };
	const auto b = jngl::Color(0x68, 0xda, 0x4f);
	const auto c = jngl::Color(104, 218, 79);
	REQUIRE(a.getRed() == b.getRed());
	REQUIRE(c.getRed() == b.getRed());
	REQUIRE(a.getGreen() == b.getGreen());
	REQUIRE(c.getGreen() == b.getGreen());
	REQUIRE(a.getBlue() == b.getBlue());
	REQUIRE(c.getBlue() == b.getBlue());
}

TEST_CASE("Rgb") { // NOLINT
	const jngl::Rgb a = 0x68da4f_rgb;
	const auto b = jngl::Rgb::u8(0x68, 0xda, 0x4f);
	const auto c = jngl::Rgb::u8(104, 218, 79);
	REQUIRE(a.getRed() == b.getRed());
	REQUIRE(c.getRed() == b.getRed());
	REQUIRE(a.getGreen() == b.getGreen());
	REQUIRE(c.getGreen() == b.getGreen());
	REQUIRE(a.getBlue() == b.getBlue());
	REQUIRE(c.getBlue() == b.getBlue());
}
