// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <jngl/ImageData.hpp>
#include <jngl/Sprite.hpp>
#include <jngl/matrix.hpp>
#include <jngl/shapes.hpp>
#include <vector>

namespace {

/// White, but with black columns where x % 8 is 3 or 4. Drawn at an eighth of its size, plain
/// bilinear filtering samples right between those two columns and gets black. The mipmap of that
/// size averages each 8 columns into one instead, which is 75 % white.
class Stripes : public jngl::ImageData {
public:
	Stripes() : bytes(static_cast<size_t>(size) * size * 4, 255) {
		for (size_t y = 0; y < size; ++y) {
			for (size_t x = 0; x < size; ++x) {
				if (x % 8 == 3 || x % 8 == 4) {
					for (size_t channel = 0; channel < 3; ++channel) {
						bytes[(y * size + x) * 4 + channel] = 0;
					}
				}
			}
		}
	}
	int getWidth() const override {
		return size;
	}
	int getHeight() const override {
		return size;
	}
	int getImageWidth() const override {
		return size;
	}
	int getImageHeight() const override {
		return size;
	}
	const uint8_t* pixels() const override {
		return bytes.data();
	}

private:
	static constexpr int size = 256;
	std::vector<uint8_t> bytes;
};

} // namespace

TEST_CASE("mipmaps") {
	Fixture f(1.f);
	const jngl::Vec2 center{ -40, 0 };
	jngl::drawRect(jngl::modelview().translate(center - jngl::Vec2(16, 16)), { 32, 32 },
	               0xbfbfbf_rgb);
	const auto expected = f.getAsciiArt();
	Fixture::reset();

	const Stripes stripes;
	const auto eighth = jngl::modelview().translate(center).scale(1. / 8);
	{
		const jngl::Sprite sprite(stripes, 1., std::nullopt, true);
		sprite.draw(eighth);
		REQUIRE(f.getAsciiArt() == expected);
		Fixture::reset();
	}

	// Mipmaps enabled for a texture that has already been loaded (and drawn in this frame)
	const jngl::Sprite withoutMipmaps(stripes, 1., "MipmapTest", false);
	withoutMipmaps.draw(eighth);
	REQUIRE(f.getAsciiArt() != expected); // black
	const jngl::Sprite withMipmaps(stripes, 1., "MipmapTest", true);
	withoutMipmaps.draw(eighth); // shares the texture, which has mipmaps now
	REQUIRE(f.getAsciiArt() == expected);
	jngl::unload("MipmapTest");
}
