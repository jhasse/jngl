// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../jngl/Channel.hpp"
#include "../jngl/sound.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Channel") {
	jngl::Channel channel;
	CHECK(!channel.isPlaying("test.ogg"));
	CHECK(!jngl::Channel::main().isPlaying("test.ogg"));
	jngl::play("test.ogg");
	CHECK(!channel.isPlaying("test.ogg"));
	CHECK(jngl::Channel::main().isPlaying("test.ogg"));
	channel.play("test.ogg");
	CHECK(channel.isPlaying("test.ogg"));
	CHECK(jngl::Channel::main().isPlaying("test.ogg"));
	jngl::Channel::main().stop("test.ogg");
	CHECK(channel.isPlaying("test.ogg"));
	CHECK(!jngl::Channel::main().isPlaying("test.ogg"));
	channel.stop("test.ogg");
	CHECK(!channel.isPlaying("test.ogg"));
	CHECK(!jngl::Channel::main().isPlaying("test.ogg"));
	CHECK_THROWS_AS(channel.play("nonexistent.ogg"), std::runtime_error);
	CHECK_THROWS_AS(channel.stop("nonexistent.ogg"), std::runtime_error);
	CHECK_THROWS_AS(channel.isPlaying("nonexistent.ogg"), std::runtime_error);
	channel.play("test.ogg");
	channel = jngl::Channel{};
	CHECK(!channel.isPlaying("test.ogg"));
}
