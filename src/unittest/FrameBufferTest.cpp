// Copyright 2020-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/FrameBuffer.hpp"
#include "../jngl/ScaleablePixels.hpp"
#include "../jngl/Sprite.hpp"
#include "../jngl/matrix.hpp"
#include "../jngl/shapes.hpp"
#include "Fixture.hpp"

#include <boost/ut.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut;
	"FrameBuffer"_test = [] {
		Fixture f(1.f);
		jngl::FrameBuffer fb(320_px, 70_px);
		jngl::drawRect({ -10, 0 }, { 10, 10 });
		{
			const auto context = fb.use();
			jngl::setAlpha(150);
			jngl::drawRect({ 10, 0 }, { 30, 10 });
			jngl::setAlpha(255);
			jngl::drawRect({ 40, 0 }, { 30, 10 });
		}
		expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒               ▒
▒              ▒               ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		jngl::drawRect({ -10, 0 }, { 10, 10 });
		fb.draw(-160, -35);
		expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒ ░░░▒▒▒        ▒
▒              ▒ ░░░▒▒▒        ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		jngl::setSpriteAlpha(150);
		fb.draw(-160, -35);
		jngl::setSpriteAlpha(255);
		jngl::drawRect({ -10, 0 }, { 10, 10 });
		expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒    ░░░        ▒
▒              ▒    ░░░        ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));

		jngl::FrameBuffer fb2(320_px, 70_px);
		{
			const auto context1 = fb.use();
			jngl::drawRect({ -40, 0 }, { 10, 10 });
			{
				const auto context2 = fb2.use();
				jngl::drawRect({ -10, -20 }, { 10, 10 });
			}
			jngl::drawRect({ -80, 0 }, { 10, 10 });
		}
		fb2.draw(-160, -35);
		expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒              ▒               ▒
▒              ▒               ▒
▒                              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));

		// Check if drawing on fb *after* fb2 was in use worked
		fb.draw(-160, -35);
		expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒       ▒   ▒    ░░░▒▒▒        ▒
▒       ▒   ▒    ░░░▒▒▒        ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));

		{
			auto context = fb.use();
			context.clear();
			jngl::setAlpha(255);
			jngl::drawCircle(jngl::Vec2{ 0, 0 }, 15);
		}
		jngl::drawCircle(jngl::modelview().translate({ 100, 0 }),
		                 6); // check that the background of the framebuffer is transparent
		fb.draw(-160, -35);
		expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒             ░██░             ▒
▒             ▒██▒       ▒▒    ▒
▒             ░██░             ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		{
			auto context = fb.use();
			context.clear(0xdd2222_rgb);
			jngl::drawCircle(jngl::Vec2{ 40, 0 }, 15);
		}
		fb.draw(-160, -35);
		expect(eq(f.getAsciiArt(), std::string(R"(
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓██▓▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
)")));
	};

	"FrameBufferScale"_test = [] {
		for (float scaleFactor : { 1.f, 5.f }) {
			Fixture f(scaleFactor);
			// check if scaling is correct when using a small FrameBuffer
			jngl::FrameBuffer smallFb(100_sp, 30_sp);
			jngl::drawRect({ -10, 0 }, { 10, 10 });
			{
				const auto context = smallFb.use();
				jngl::setAlpha(150);
				jngl::drawRect({ 10, 0 }, { 30, 10 });
				jngl::setAlpha(255);
				jngl::drawRect({ 40, 0 }, { 30, 10 });
			}
			smallFb.draw(-160, 0);
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒              ▒               ▒
▒              ▒               ▒
▒     ░░░█                     ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
	};
};
} // namespace
