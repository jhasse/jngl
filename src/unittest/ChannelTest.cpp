// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../jngl/Channel.hpp"
#include "../jngl/sound.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Channel") {
	jngl::Channel channel;
	CHECK(!channel.isPlaying("../data/test.ogg"));
	CHECK(!jngl::Channel::main().isPlaying("../data/test.ogg"));
	jngl::play("../data/test.ogg");
	CHECK(!channel.isPlaying("../data/test.ogg"));
	CHECK(jngl::Channel::main().isPlaying("../data/test.ogg"));
	channel.play("../data/test.ogg");
	CHECK(channel.isPlaying("../data/test.ogg"));
	CHECK(jngl::Channel::main().isPlaying("../data/test.ogg"));
	jngl::Channel::main().stop("../data/test.ogg");
	CHECK(channel.isPlaying("../data/test.ogg"));
	CHECK(!jngl::Channel::main().isPlaying("../data/test.ogg"));
	channel.stop("../data/test.ogg");
	CHECK(!channel.isPlaying("../data/test.ogg"));
	CHECK(!jngl::Channel::main().isPlaying("../data/test.ogg"));
	CHECK_THROWS_AS(channel.play("nonexistent.ogg"), std::runtime_error);
	CHECK_THROWS_AS(channel.stop("nonexistent.ogg"), std::runtime_error);
	CHECK_THROWS_AS(channel.isPlaying("nonexistent.ogg"), std::runtime_error);
}
