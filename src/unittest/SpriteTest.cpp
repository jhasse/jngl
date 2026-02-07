// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fixture.hpp"

#include "../TextureCache.hpp"

#include <boost/ut.hpp>
#include <jngl.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT
	"Sprite"_test = [] {
		for (float factor : { 1.f, 2.f, 3.4f }) {
			Fixture f(factor);
			jngl::Sprite sprite("../data/jngl.webp");
			sprite.setPos(-60, -30);
			sprite.draw(jngl::modelview().scale(0.2f, 0.2f));
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
			jngl::load("../data/jngl.webp"); // This shouldn't crash
		}
		{
			Fixture f(3.4f);
			jngl::Sprite sprite("../data/jngl.webp");
			expect(approx(sprite.getWidth(), 600, 1e-9));
			expect(approx(sprite.getHeight(), 300, 1e-9));

			expect(approx(sprite.getSize().x, 600, 1e-4)); // rounding errors are strong in this one
			expect(approx(sprite.getSize().y, 300, 1e-4));

			sprite.setCenter(7.3f, 9.1f);
			expect(approx(boost::qvm::mag(sprite.getCenter() - jngl::Vec2(7.3f, 9.1f)), 0, 1e-5));
			expect(approx(sprite.getX(), -292.7f, 1e-4));
			expect(approx(sprite.getY(), -140.9f, 1e-5));
			expect(approx(sprite.getLeft(), -132.7f, 1e-5));
			expect(approx(sprite.getTop(), -105.9f, 1e-5));
			expect(approx(sprite.getRight(), -147.3f, 1e-5));
			expect(approx(sprite.getBottom(), -124.1f, 1e-5));
		}
	};
	"batch"_test = [] {
		Fixture f(2.0f);
		jngl::Sprite sprite("../data/jngl.webp");
		sprite.setPos(-60, -30);
		{
			auto batch = sprite.batch();
			batch.draw(jngl::modelview().translate({ -90, 0 }).scale(0.2f));
			batch.draw(jngl::modelview().translate({ 80, 0 }).scale(0.2f));
		}
		expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒    ░░░░             ░░░░     ▒
▒  ░░░░░░░░         ░░░░░░░░   ▒
▒   ░░░░░░           ░░░░░░    ▒
▒     ░░               ░░      ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
	};
	"TextureCache"_test = [] {
		{
			Fixture f(1.9f);
			expect(jngl::TextureCache::handle().sprites.empty());
			expect(eq(jngl::TextureCache::handle().get("../data/jngl.webp"), nullptr));
			jngl::load("../data/jngl.webp");
			expect(eq(jngl::TextureCache::handle().sprites.size(), 1u));
			expect(neq(jngl::TextureCache::handle().get("../data/jngl.webp"), nullptr));
			jngl::Sprite sprite("../data/jngl.webp");
			expect(eq(jngl::TextureCache::handle().sprites.size(), 1u));
			expect(neq(jngl::TextureCache::handle().get("../data/jngl.webp"), nullptr));
			sprite.draw(jngl::modelview().scale(0.2f));
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
		{
			expect(jngl::TextureCache::handle().sprites.empty());
			expect(eq(jngl::TextureCache::handle().get("../data/jngl.webp"), nullptr));
			Fixture f(1.9f);
			jngl::Sprite sprite("../data/jngl.webp");
			sprite.draw(jngl::modelview().scale(0.2f));
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
	};
	"Loader"_test = []() {
		for (float factor : { 1.f, 2.f, 3.4f }) {
			Fixture f(factor);
			jngl::Sprite::Loader loader("../data/jngl.webp");
			loader->setPos(-60, -30);
			expect(static_cast<bool>(loader));
			loader->draw(jngl::modelview().scale(0.2f, 0.2f));
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░░░░             ▒
▒           ░░░░░░░░           ▒
▒            ░░░░░░            ▒
▒              ░░              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
	};
	"drawClipped"_test = [] {
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
		expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒           ░                  ▒
▒    ░░░░░░░▒       ░░░░ ░░▒   ▒
▒    ░░░░▒▒░▒▒░░    ░░░░░▒░▒░░ ▒
▒  ░░░░░░░░░░░▒░  ░░░░░░░░░░░░░▒
▒  ░▒░░░░░░░░░▒░  ░▒░░░░░░░░░▒░▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
	};
};
} // namespace
