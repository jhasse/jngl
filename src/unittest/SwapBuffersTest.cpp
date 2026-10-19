// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../main.hpp"

#include <catch2/catch_test_macros.hpp>
#include <jngl.hpp>

namespace {

/// The color of the window's pixel at \a x, \a y from the top left, as 0 to 255 (red only, as
/// everything's gray here)
int pixel(const int x, const int y) {
	const auto pixels = jngl::readPixels();
	const int width = jngl::getWindowWidth();
	const int fromBottom = jngl::getWindowHeight() - 1 - y;
	return static_cast<int>(pixels.at(static_cast<size_t>(fromBottom * width + x) * 3) * 255.f +
	                        .5f);
}

} // namespace

TEST_CASE("drawing before the first swapBuffers()") {
	jngl::hideWindow();
	jngl::internal::setHighDpi(false);
	jngl::setScaleFactor(1);
	jngl::showWindow("unit test", 320, 70, false, { 32, 7 }, { 32, 7 });

	// Like after any swapBuffers(), it's drawn onto the cleared window
	jngl::drawRect(jngl::modelview().translate({ -160, -35 }), { 160, 70 }, 0x000000_rgb);
	REQUIRE(pixel(80, 35) == 0);
	REQUIRE(pixel(240, 35) == 255);

	// Clears it again, as does swapBuffers()
	jngl::clearBackBuffer();
	REQUIRE(pixel(80, 35) == 255);

	// Clears it right away, too
	jngl::setBackgroundColor(0x000000_rgb);
	REQUIRE(pixel(240, 35) == 0);

	jngl::hideWindow();
}
