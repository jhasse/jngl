// Copyright 2018-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fixture.hpp"

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
};
} // namespace
