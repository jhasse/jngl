// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fixture.hpp"

#include "../TextureCache.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <jngl.hpp>

TEST_CASE("Sprite") {
	for (float factor : { 1.f, 2.f, 3.4f }) {
		Fixture f(factor);
		jngl::Sprite sprite("../data/jngl.webp");
		sprite.setPos(-60, -30);
		sprite.draw(jngl::modelview().scale(0.2f, 0.2f));
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
		jngl::load("../data/jngl.webp"); // This shouldn't crash
	}
	{
		Fixture f(3.4f);
		jngl::Sprite sprite("../data/jngl.webp");
		REQUIRE_THAT(sprite.getWidth(), Catch::Matchers::WithinAbs(600, 1e-9));
		REQUIRE_THAT(sprite.getHeight(), Catch::Matchers::WithinAbs(300, 1e-9));

		REQUIRE_THAT(sprite.getSize().x, Catch::Matchers::WithinAbs(
		                                     600, 1e-4)); // rounding errors are strong in this one
		REQUIRE_THAT(sprite.getSize().y, Catch::Matchers::WithinAbs(300, 1e-4));

		sprite.setCenter(7.3f, 9.1f);
		REQUIRE_THAT(boost::qvm::mag(sprite.getCenter() - jngl::Vec2(7.3f, 9.1f)),
		             Catch::Matchers::WithinAbs(0, 1e-5));
		REQUIRE_THAT(sprite.getX(), Catch::Matchers::WithinAbs(-292.7f, 1e-4));
		REQUIRE_THAT(sprite.getY(), Catch::Matchers::WithinAbs(-140.9f, 1e-5));
		REQUIRE_THAT(sprite.getLeft(), Catch::Matchers::WithinAbs(-132.7f, 1e-5));
		REQUIRE_THAT(sprite.getTop(), Catch::Matchers::WithinAbs(-105.9f, 1e-5));
		REQUIRE_THAT(sprite.getRight(), Catch::Matchers::WithinAbs(-147.3f, 1e-5));
		REQUIRE_THAT(sprite.getBottom(), Catch::Matchers::WithinAbs(-124.1f, 1e-5));
	}
}
TEST_CASE("Sprite_Batch") {
	Fixture f(2.0f);
	jngl::Sprite sprite("../data/jngl.webp");
	sprite.setPos(-60, -30);
	{
		auto batch = sprite.batch();
		batch.draw(jngl::modelview().translate({ -90, 0 }).scale(0.2f));
		batch.draw(jngl::modelview().translate({ 80, 0 }).scale(0.2f));
	}
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒    ░░░░             ░░░░     ▒
▒  ░░░░░░░░         ░░░░░░░░   ▒
▒   ░░░░░░           ░░░░░░    ▒
▒     ░░               ░░      ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}
TEST_CASE("TextureCache") {
	{
		Fixture f(1.9f);
		REQUIRE(jngl::TextureCache::handle().sprites.empty());
		REQUIRE(jngl::TextureCache::handle().get("../data/jngl.webp") == nullptr);
		jngl::load("../data/jngl.webp");
		REQUIRE(jngl::TextureCache::handle().sprites.size() == 1u);
		REQUIRE(jngl::TextureCache::handle().get("../data/jngl.webp") != nullptr);
		jngl::Sprite sprite("../data/jngl.webp");
		REQUIRE(jngl::TextureCache::handle().sprites.size() == 1u);
		REQUIRE(jngl::TextureCache::handle().get("../data/jngl.webp") != nullptr);
		sprite.draw(jngl::modelview().scale(0.2f));
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
	{
		REQUIRE(jngl::TextureCache::handle().sprites.empty());
		REQUIRE(jngl::TextureCache::handle().get("../data/jngl.webp") == nullptr);
		Fixture f(1.9f);
		jngl::Sprite sprite("../data/jngl.webp");
		sprite.draw(jngl::modelview().scale(0.2f));
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
}
TEST_CASE("Loader") {
	for (float factor : { 1.f, 2.f, 3.4f }) {
		Fixture f(factor);
		jngl::Sprite::Loader loader("../data/jngl.webp");
		loader->setPos(-60, -30);
		REQUIRE(static_cast<bool>(loader));
		loader->draw(jngl::modelview().scale(0.2f, 0.2f));
		REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	}
}
TEST_CASE("drawClipped") {
	Fixture f(1.5f);
	jngl::scale(0.4f);
	jngl::Sprite sprite("../data/jngl.webp");
	sprite.setPos(-310, -50);
	sprite.drawClipped({ 0.25f, 0.25f }, { 0.75f, 0.75f });
	sprite.setPos(-90, -120);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
	sprite.drawClipped(0.25f, 0.75f, 0.25f, 0.75f);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒           ░                  ▒
▒    ░░░░░░░▒       ░░░░ ░░▒   ▒
▒    ░░░░▒▒░▒▒░░    ░░░░░▒░▒░░ ▒
▒  ░░░░░░░░░░░▒░  ░░░░░░░░░░░░░▒
▒  ░▒░░░░░░░░░▒░  ░▒░░░░░░░░░▒░▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}
