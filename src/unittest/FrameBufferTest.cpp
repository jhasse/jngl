// Copyright 2020-2026 Jan Niklas Hasse <jhasse@bixense.com>
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
				jngl::translate(-10, 0); // FrameBuffer::use() should push/pop the modelview matrix
				jngl::drawRect({ 0, -20 }, { 10, 10 });
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

			// check drawMesh works with scaling
			{
				auto context = smallFb.use();
				context.clear(0xdd2222_rgb);
				jngl::drawCircle(jngl::Vec2{ 10, 0 }, 15);
			}
			const float width = smallFb.getSize().x;
			const float height = smallFb.getSize().y;
			const float x = -100;
			const float y = -20;
			smallFb.drawMesh({
			    // top right triangle
			    jngl::Vertex{ .x = x, .y = y, .u = 0, .v = 1 },
			    jngl::Vertex{ .x = x + width, .y = y, .u = 1, .v = 1 },
			    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },

			    // bottom left triangle
			    jngl::Vertex{ .x = x, .y = y, .u = 0, .v = 1 },
			    jngl::Vertex{ .x = x, .y = y + height, .u = 0, .v = 0 },
			    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },

			    // top right triangle
			    jngl::Vertex{ .x = x, .y = y + height, .u = 0, .v = 0 },
			    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },
			    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },

			    // bottom left triangle
			    jngl::Vertex{ .x = x, .y = y + height, .u = 0, .v = 0 },
			    jngl::Vertex{ .x = x, .y = y + height, .u = 0, .v = 0 },
			    jngl::Vertex{ .x = x + width, .y = y + height, .u = 1, .v = 0 },
			});
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒     ░░░░░▒▒░░░               ▒
▒     ▒▒▒▒▓██▓▒▒               ▒
▒     ▒▒▒▒▓██▓▒▒               ▒
▒     ░░░░░▒▒░░░               ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
	};

	"FrameBufferInception"_test = [] {
		// create a FrameBuffer while another FrameBuffer is bound:
		Fixture f{ 1 };
		std::optional<jngl::FrameBuffer> fb1 = jngl::FrameBuffer(30_sp, 30_sp);
		std::optional<jngl::FrameBuffer> fb2;
		{
			const auto context = fb1->use();
			fb2.emplace(30_sp, 30_sp);
		}
		fb1 = {};
		{
			const auto context = fb2->use();
			jngl::drawRect({ 10, 0 }, { 10, 10 });
		}
		fb2->draw(0, 0);
		expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                              ▒
▒                              ▒
▒                              ▒
▒                 ▒            ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
	};
};
} // namespace
