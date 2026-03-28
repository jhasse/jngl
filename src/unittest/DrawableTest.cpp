// Copyright 2019-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Drawable.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

class Dummy : public jngl::Drawable {
public:
	void step() override {
	}
	void draw() const override {
	}
};

TEST_CASE("Drawable") {
	Dummy d;
	REQUIRE_THAT(d.getX(), Catch::Matchers::WithinAbs(0, 1e-9));
	REQUIRE_THAT(d.getY(), Catch::Matchers::WithinAbs(0, 1e-9));
	REQUIRE_THAT(d.getWidth(), Catch::Matchers::WithinAbs(0, 1e-9));
	REQUIRE_THAT(d.getHeight(), Catch::Matchers::WithinAbs(0, 1e-9));
	REQUIRE_THAT(d.getWidth(), Catch::Matchers::WithinAbs(d.getSize().x, 1e-9));
	REQUIRE_THAT(d.getHeight(), Catch::Matchers::WithinAbs(d.getSize().y, 1e-9));
}
