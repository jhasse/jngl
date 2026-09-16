// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Vec2.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <numbers>

TEST_CASE("Vec2") {
	//
	//  o-->
	//
	jngl::Vec2 a{ 1., 0. };

#if __has_include(<format>) && (!defined(_LIBCPP_VERSION) || _LIBCPP_VERSION >= 170000)
	REQUIRE(std::format("{}", a) == "[x=1, y=0]");
#endif

	a.rotate(std::numbers::pi / 2);
	//
	//  o
	//  |
	//  v
	//
	REQUIRE_THAT(a.x, Catch::Matchers::WithinAbs(0., 1e-6));
	REQUIRE_THAT(a.y, Catch::Matchers::WithinAbs(
	                      1., 1e-6)); // JNGL uses negative y for up and positive for down.

	a.rotate(-std::numbers::pi / 4);
	//
	//  o
	//   ╲
	//    ⌟
	//
	REQUIRE_THAT(a.x, Catch::Matchers::WithinAbs(std::numbers::sqrt2 / 2., 1e-6));
	REQUIRE_THAT(a.y, Catch::Matchers::WithinAbs(std::numbers::sqrt2 / 2., 1e-6));
	a.rotate(0);
	REQUIRE_THAT(a.x, Catch::Matchers::WithinAbs(std::numbers::sqrt2 / 2., 1e-6));
	REQUIRE_THAT(a.y, Catch::Matchers::WithinAbs(std::numbers::sqrt2 / 2., 1e-6));
	a.rotate(std::numbers::pi * 8);
	REQUIRE_THAT(a.x, Catch::Matchers::WithinAbs(std::numbers::sqrt2 / 2., 1e-6));
	REQUIRE_THAT(a.y, Catch::Matchers::WithinAbs(std::numbers::sqrt2 / 2., 1e-6));
}
