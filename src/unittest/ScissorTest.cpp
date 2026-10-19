// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/FrameBuffer.hpp"
#include "../jngl/ScaleablePixels.hpp"
#include "../jngl/matrix.hpp"
#include "../jngl/other.hpp"
#include "../jngl/shapes.hpp"
#include "../jngl/window.hpp"
#include "../opengl.hpp"
#include "../window.hpp"
#include "../windowptr.hpp"
#include "Fixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace {

/// Covers the whole 320x70 window (or a FrameBuffer that size), so that only what the scissor lets
/// through shows
void fill(const jngl::Rgb color) {
	jngl::drawRect(jngl::modelview().translate({ -160, -35 }), { 320, 70 }, color);
}

/// The ASCII art's cell in \a column and \a row, as the character it's drawn with: a space or
/// one of the three-byte block characters
std::string cell(const std::string& art, const size_t column, const size_t row) {
	std::istringstream lines(art);
	std::string line;
	for (size_t i = 0; i <= row + 1; ++i) { // the art starts with a newline
		std::getline(lines, line);
	}
	const auto width = [&line](const size_t pos) -> size_t {
		return (static_cast<unsigned char>(line.at(pos)) & 0x80) ? 3 : 1;
	};
	size_t pos = 0;
	for (size_t i = 0; i < column; ++i) {
		pos += width(pos);
	}
	return line.substr(pos, width(pos));
}

/// Whether OpenGL's scissor test is on, and its box: x and y from the bottom left, width, height
using ScissorState = std::tuple<bool, GLint, GLint, GLint, GLint>;
[[maybe_unused]] ScissorState scissorState() {
	std::array<GLint, 4> box{};
	glGetIntegerv(GL_SCISSOR_BOX, box.data());
	return { glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE, box[0], box[1], box[2], box[3] };
}

/// OpenGL's scissor state can't be queried on the Vulkan backend, only what gets drawn
#ifdef JNGL_VULKAN
#define REQUIRE_SCISSOR_STATE(...)
#else
#define REQUIRE_SCISSOR_STATE(...) REQUIRE(__VA_ARGS__)
#endif

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
	REQUIRE_SCISSOR_STATE(!std::get<0>(scissorState()));
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
	[[maybe_unused]] const ScissorState letterboxing{ true, 80, 0, 160, 35 };
	REQUIRE_SCISSOR_STATE(scissorState() == letterboxing);

	// A FrameBuffer that covers the screen, i.e. of the canvas' pixels before it was zoomed out,
	// which it's drawn into at getScaleFactor() pixels per screen unit rather than the zoomed half
	jngl::FrameBuffer fb{ jngl::getWindowSize() };
	{
		const auto scissor = jngl::scissor({ -110, -25 }, { 120, 40 });
		// Inside the canvas, right of its bar, at half the size
		[[maybe_unused]] const ScissorState onWindow{ true, 105, 10, 60, 20 };
		REQUIRE_SCISSOR_STATE(scissorState() == onWindow);
		{
			const auto context = fb.use();
			// The same place on the FrameBuffer, which has no bars, at full size: where a
			// rectangle drawn there lands
			REQUIRE_SCISSOR_STATE(scissorState() == ScissorState{ true, 50, 20, 120, 40 });
			jngl::drawRect(jngl::modelview().translate({ -110, -25 }), { 120, 40 }, 0x000000_rgb);
#ifndef JNGL_VULKAN // glReadPixels can't read a FrameBuffer there
			std::vector<unsigned char> pixels(size_t{ 320 } * 70 * 4);
			glReadPixels(0, 0, 320, 70, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
			const auto isDrawn = [&](const int x, const int y) {
				return pixels.at(static_cast<size_t>(y * 320 + x) * 4) < 128;
			};
			REQUIRE((isDrawn(50, 20) && isDrawn(169, 59)));
			REQUIRE(
			    (!isDrawn(49, 20) && !isDrawn(170, 59) && !isDrawn(50, 19) && !isDrawn(169, 60)));
#endif
		}
		REQUIRE_SCISSOR_STATE(scissorState() == onWindow);
		fill(0x000000_rgb);
	}
	REQUIRE_SCISSOR_STATE(scissorState() == letterboxing);
	{
		const auto context = fb.use();
		// the letterboxing doesn't apply to a FrameBuffer
		REQUIRE_SCISSOR_STATE(!std::get<0>(scissorState()));
	}
	REQUIRE_SCISSOR_STATE(scissorState() == letterboxing);
	// readPixels() zooms the canvas back in with linear filtering, which smudges the fill's edges
	// into the cells next to them by an amount that differs between drivers, e.g. where its top
	// meets the frame on GitHub's runners. So only the cells well inside it, columns 5 to 16 and
	// rows 1 to 4, and well outside it are looked at.
	const auto art = f.getAsciiArt();
	for (size_t row = 1; row <= 4; ++row) {
		for (size_t column = 6; column <= 15; ++column) {
			REQUIRE(cell(art, column, row) == "█");
		}
		for (size_t column = 18; column <= 29; ++column) {
			REQUIRE(cell(art, column, row) == " ");
		}
		REQUIRE(cell(art, 2, row) == " ");
	}
	REQUIRE(cell(art, 10, 6) == "▒"); // the frame, far from the fill
}
