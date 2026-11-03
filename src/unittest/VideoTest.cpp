// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <jngl/Video.hpp>
#include <jngl/matrix.hpp>
#include <memory>
#include <stdexcept>

TEST_CASE("Video") {
	Fixture f(1.f);
	std::unique_ptr<jngl::Video> video;
	try {
		video = std::make_unique<jngl::Video>("verysmall.ogv");
	} catch (const std::runtime_error& e) {
		SKIP(e.what()); // JNGL compiled without video support
	}
	// Its first frame, as it doesn't start playing until enough of it has been decoded, scaled
	// down to the window's height
	jngl::pushMatrix();
	jngl::scale(70. / 320.);
	video->draw();
	jngl::popMatrix();
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓██▒▒▒▒▒▒▒▒▒▓
▒         ▓▓▓▓▓▓▓▓▓▓▓▓         ▒
▒         ▓▒▒▒▓▒▓▓▓▓▓▓         ▒
▒         ▒▒▓▒▒▓▒▒▒▒▓▓         ▒
▒         ▒▒▓▒▓▓▓▒▓▒▒▒         ▒
▒         ▒▒▒▒▓▓▒▓▓▒▒▒         ▒
▓▒▒▒▒▒▒▒▒▒░░░░░▒▒░░▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	// Still known when no frame is left over from decoding
	REQUIRE(video->getWidth() == 560);
	REQUIRE(video->getHeight() == 320);
}
