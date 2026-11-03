// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fixture.hpp"

#include "../TextureCache.hpp"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstdint>
#include <jngl.hpp>
#include <memory>
#include <vector>

TEST_CASE("Sprite") {
	for (float factor : { 1.f, 2.f, 3.4f }) {
		Fixture f(factor);
		jngl::Sprite sprite("jngl.webp");
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
		jngl::load("jngl.webp"); // This shouldn't crash
	}
	{
		Fixture f(3.4f);
		jngl::Sprite sprite("jngl.webp");
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
TEST_CASE("Sprite::setTextureFilter") {
	Fixture f(1.f);
	// A black and a white pixel, stretched over the window: smoothed in between, or not
	const std::array<uint8_t, 8> bytes{ 0, 0, 0, 255, 255, 255, 255, 255 };
	jngl::Sprite sprite(bytes.data(), 2, 1);
	const auto draw = [&]() {
		Fixture::reset();
		sprite.draw(jngl::modelview().scale(160, 70));
		return f.getAsciiArt();
	};
	const auto smooth = draw();
	sprite.setTextureFilter(jngl::TextureFilter::Bilinear, jngl::TextureFilter::NearestNeighbor);
	const auto sharp = draw();
	CHECK(smooth != sharp);
	std::string half; // black, then white
	for (int row = 0; row < 7; ++row) {
		half += "\n" + std::string("████████████████") + std::string(16, ' ');
	}
	REQUIRE(sharp == half + "\n");
	sprite.setTextureFilter(jngl::TextureFilter::Bilinear, jngl::TextureFilter::Bilinear);
	CHECK(draw() == smooth);
}

TEST_CASE("Sprite_Batch") {
	Fixture f(2.0f);
	jngl::Sprite sprite("jngl.webp");
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
TEST_CASE("drawLerped") {
	Fixture f(1.f);
	jngl::Sprite sprite("jngl.webp");

	// Lerping fully towards black has to look exactly like multiplying with black. Comparing
	// against that instead of hardcoded ASCII art, because the anti-aliased edges of the
	// black silhouette end up close to the thresholds between characters, which makes the
	// result depend on the driver's texture filtering (e.g. llvmpipe under Xvfb).
	jngl::setSpriteColor(0x000000_rgb);
	sprite.draw(jngl::modelview().scale(0.2f));
	jngl::setSpriteColor(0xffffff_rgb);
	const std::string silhouette = f.getAsciiArt();

	sprite.drawLerped(jngl::modelview().scale(0.2f), 0x000000_rgb, 1.f, 1.f, 1.f, jngl::Alpha(1.f));
	REQUIRE(f.getAsciiArt() == silhouette);

	// setSpriteColor gets ignored:
	jngl::setSpriteColor(0x000000_rgb);
	sprite.drawLerped(jngl::modelview().scale(0.2f), 0x000000_rgb, 0.f, 0.f, 0.f, jngl::Alpha(1.f));
	jngl::setSpriteColor(0xffffff_rgb);
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

	sprite.drawLerped(jngl::modelview().scale(0.2f), 0x000000_rgb, 1.f, 1.f, 1.f, jngl::Alpha(.5f));
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒           ░░░░▒▒░            ▒
▒           ░▒▒▒▒▒░            ▒
▒           ░▒▒░▒▒▒░           ▒
▒            ░░░░░░            ▒
▒             ░░░░             ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}
TEST_CASE("TextureCache") {
	{
		Fixture f(1.9f);
		REQUIRE(jngl::TextureCache::handle().sprites.empty());
		REQUIRE(jngl::TextureCache::handle().get("jngl.webp") == nullptr);
		jngl::load("jngl.webp");
		REQUIRE(jngl::TextureCache::handle().sprites.size() == 1u);
		REQUIRE(jngl::TextureCache::handle().get("jngl.webp") != nullptr);
		jngl::Sprite sprite("jngl.webp");
		REQUIRE(jngl::TextureCache::handle().sprites.size() == 1u);
		REQUIRE(jngl::TextureCache::handle().get("jngl.webp") != nullptr);
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
		REQUIRE(jngl::TextureCache::handle().get("jngl.webp") == nullptr);
		Fixture f(1.9f);
		jngl::Sprite sprite("jngl.webp");
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
		jngl::Sprite::Loader loader("jngl.webp");
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
	jngl::Sprite sprite("jngl.webp");
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

TEST_CASE("more Sprites than fit into the Vulkan backend's first descriptor pool") {
	Fixture f(1.f);
	// It holds 1024. Each draws a black 2x2 pixel square in a 30 by 30 grid (60x60 pixels).
	const std::vector<uint8_t> black{ 0, 0, 0, 255 };
	std::vector<std::unique_ptr<jngl::Sprite>> sprites;
	const auto drawAll = [&]() {
		for (size_t i = 0; i < sprites.size(); ++i) {
			const auto x = static_cast<double>(i % 30) * 2. - 30.;
			const auto y = static_cast<double>(i / 30 % 30) * 2. - 30.;
			sprites[i]->draw(jngl::modelview().translate({ x + 1, y + 1 }).scale(2));
		}
	};
	for (int i = 0; i < 1100; ++i) {
		sprites.emplace_back(std::make_unique<jngl::Sprite>(black.data(), 1, 1));
	}
	drawAll();
	const auto expected = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒                              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
	const auto art = f.getAsciiArt();
	// Frees some in the first pool and some in the second, then creates as many again
	sprites.erase(sprites.begin() + 1000, sprites.begin() + 1050);
	sprites.erase(sprites.begin(), sprites.begin() + 50);
	for (int i = 0; i < 100; ++i) {
		sprites.emplace_back(std::make_unique<jngl::Sprite>(black.data(), 1, 1));
	}
	drawAll();
	REQUIRE(f.getAsciiArt() == art);
	REQUIRE(art != expected); // the squares show
}
