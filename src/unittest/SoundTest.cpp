// Copyright 2024-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include <catch2/catch_test_macros.hpp>
#include <jngl/other.hpp>
#include <jngl/sound.hpp>
#include <jngl/SoundFile.hpp>

TEST_CASE("Sound") {
	REQUIRE(!jngl::isPlaying("test.ogg"));
	jngl::play("test.ogg");
	REQUIRE(jngl::isPlaying("test.ogg"));
	jngl::stop("test.ogg");
	REQUIRE(!jngl::isPlaying("test.ogg"));
	REQUIRE_THROWS_AS(jngl::play("nonexistent.ogg"), std::runtime_error);
	REQUIRE_THROWS_AS(jngl::stop("nonexistent.ogg"), std::runtime_error);
	REQUIRE_THROWS_AS(jngl::isPlaying("nonexistent.ogg"), std::runtime_error);
}

TEST_CASE("SoundFile") {
	const auto path = jngl::getPrefix() + "test.ogg";
	{
		// Decoded on its own thread, which load() waits for
		jngl::SoundFile soundFile(path, std::launch::async);
		soundFile.load();
		REQUIRE(soundFile.isLoaded());
		REQUIRE(soundFile.length().count() > 0);
	}
	{
		// Only once it's needed
		jngl::SoundFile soundFile(path, std::launch::deferred);
		REQUIRE(!soundFile.isLoaded());
		REQUIRE(soundFile.length().count() > 0);
		REQUIRE(soundFile.isLoaded());
	}
	{
		// Moved from while it's still being decoded
		jngl::SoundFile first(path, std::launch::async);
		const jngl::SoundFile second(std::move(first));
		REQUIRE(second.length().count() > 0);
	}
	// Not when it's created, but whenever it's loaded
	jngl::SoundFile missing(jngl::getPrefix() + "nonexistent.ogg", std::launch::async);
	REQUIRE_THROWS_AS(missing.load(), std::runtime_error);
	REQUIRE_THROWS_AS(missing.load(), std::runtime_error);
	REQUIRE(missing.isLoaded());
}
