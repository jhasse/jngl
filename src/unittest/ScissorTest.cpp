// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/FrameBuffer.hpp"
#include "../jngl/ScaleablePixels.hpp"
#include "../jngl/matrix.hpp"
#include "../jngl/other.hpp"
#include "../jngl/shapes.hpp"
#include "../opengl.hpp"
#include "../window.hpp"
#include "../windowptr.hpp"
#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <tuple>

namespace {

/// Covers the whole 320x70 window (or a FrameBuffer that size), so that only what the scissor lets
/// through shows
void fill(const jngl::Rgb color) {
	jngl::drawRect(jngl::modelview().translate({ -160, -35 }), { 320, 70 }, color);
}

/// Whether OpenGL's scissor test is on, and its box: x and y from the bottom left, width, height
using ScissorState = std::tuple<bool, GLint, GLint, GLint, GLint>;
ScissorState scissorState() {
	std::array<GLint, 4> box{};
	glGetIntegerv(GL_SCISSOR_BOX, box.data());
	return { glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE, box[0], box[1], box[2], box[3] };
}

} // namespace

TEST_CASE("scissor clips to where nested ones overlap") {
	Fixture f(1.f);
	{
		// Columns 5 to 16 and rows 1 to 4 of the ASCII art
		const auto outer = jngl::scissor({ -110, -25 }, { 120, 40 });
		fill(0x000000_rgb);
	}
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒    ████████████              ▒
▒    ████████████              ▒
▒    ████████████              ▒
▒    ████████████              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	{
		const auto outer = jngl::scissor({ -110, -25 }, { 120, 40 });
		// Columns 3 to 8 and rows 3 to 5, so only columns 5 to 8 of rows 3 and 4 are in both
		const auto inner = jngl::scissor({ -130, -5 }, { 60, 30 });
		fill(0x000000_rgb);
	}
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒    ████                      ▒
▒    ████                      ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
	REQUIRE(!std::get<0>(scissorState()));
	fill(0x000000_rgb); // nothing clips it anymore
	REQUIRE(f.getAsciiArt() == R"(
████████████████████████████████
████████████████████████████████
████████████████████████████████
████████████████████████████████
████████████████████████████████
████████████████████████████████
████████████████████████████████
)");
}

TEST_CASE("scissor applies inside a FrameBuffer and after it") {
	Fixture f(1.f);
	jngl::FrameBuffer fb(320_px, 70_px);
	{
		const auto scissor = jngl::scissor({ -110, -25 }, { 120, 40 });
		{
			const auto context = fb.use();
			// Rows 1 and 2 of the ASCII art, clipped to the scissor's columns
			jngl::drawRect(jngl::modelview().translate({ -160, -25 }), { 320, 20 }, 0x000000_rgb);
		}
		// Still clipped on the window: rows 3 and 4
		jngl::drawRect(jngl::modelview().translate({ -160, -5 }), { 320, 20 }, 0x404040_rgb);
	}
	fb.draw(-160, -35); // nothing clips it now
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒    ████████████              ▒
▒    ████████████              ▒
▒    ▓▓▓▓▓▓▓▓▓▓▓▓              ▒
▒    ▓▓▓▓▓▓▓▓▓▓▓▓              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");

	// A FrameBuffer smaller than the window: (0, 0) is its center, like the screen's, and a pixel
	// of it is one of the screen's, so the scissor's right edge at x = 10 cuts it 10 pixels right
	// of its center
	jngl::FrameBuffer small(100_px, 30_px);
	{
		const auto scissor = jngl::scissor({ -110, -25 }, { 120, 40 });
		const auto context = small.use();
		jngl::drawRect(jngl::modelview().translate({ -50, -15 }), { 100, 30 }, 0x000000_rgb);
	}
	small.draw(-150, -25); // columns 1 to 10 and rows 1 to 3, black up to column 6
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒██████                        ▒
▒██████                        ▒
▒██████                        ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}

TEST_CASE("scissor and letterboxing") {
	Fixture f(1.f); // 320x70, the only aspect ratio allowed
	// Half as high, so the canvas is zoomed out to 160x35 with 80 pixel bars either side. (It
	// can't get bigger: the window's framebuffer stays what the Fixture made it.)
	jngl::pWindow->setActualSize(320, 35);
	jngl::pWindow->updateScreenSize();
	Fixture::reset();
	const ScissorState letterboxing{ true, 80, 0, 160, 35 };
	REQUIRE(scissorState() == letterboxing);

	// A FrameBuffer the canvas' size in pixels, so (0, 0) and the pixels per screen unit are the
	// same inside it
	jngl::FrameBuffer fb(160_px, 35_px);
	{
		const auto scissor = jngl::scissor({ -110, -25 }, { 120, 40 });
		// Inside the canvas, right of its bar, at half the size
		const ScissorState onWindow{ true, 105, 10, 60, 20 };
		REQUIRE(scissorState() == onWindow);
		{
			const auto context = fb.use();
			// The same place on the FrameBuffer, which has no bars
			REQUIRE(scissorState() == ScissorState{ true, 25, 10, 60, 20 });
		}
		REQUIRE(scissorState() == onWindow);
		fill(0x000000_rgb);
	}
	REQUIRE(scissorState() == letterboxing);
	{
		const auto context = fb.use();
		REQUIRE(!std::get<0>(scissorState())); // the letterboxing doesn't apply to a FrameBuffer
	}
	REQUIRE(scissorState() == letterboxing);
	// readPixels() zooms the canvas back in
	REQUIRE(f.getAsciiArt() == R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒    ████████████              ▒
▒    ████████████              ▒
▒    ████████████              ▒
▒    ████████████              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)");
}
