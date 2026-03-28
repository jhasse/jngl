// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include <catch2/catch_test_macros.hpp>
#include <jngl/sound.hpp>

TEST_CASE("Sound") {
	REQUIRE(!jngl::isPlaying("../data/test.ogg"));
	jngl::play("../data/test.ogg");
	REQUIRE(jngl::isPlaying("../data/test.ogg"));
	jngl::stop("../data/test.ogg");
	REQUIRE(!jngl::isPlaying("../data/test.ogg"));
	REQUIRE_THROWS_AS(jngl::play("nonexistent.ogg"), std::runtime_error);
	REQUIRE_THROWS_AS(jngl::stop("nonexistent.ogg"), std::runtime_error);
	REQUIRE_THROWS_AS(jngl::isPlaying("nonexistent.ogg"), std::runtime_error);
}
