// Copyright 2022-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "Fixture.hpp"

#include <boost/ut.hpp>
#include <jngl/Color.hpp>
#include <jngl/Rgba.hpp>
#include <jngl/matrix.hpp>
#include <jngl/shapes.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT
	"shapes"_test = [] {
		for (double scaleFactor : { 1, 2 }) {
			Fixture f(scaleFactor);
			auto mv = jngl::modelview();
			mv.translate({ -150, -30 });
			jngl::drawRect(mv, { 120, 40 }, jngl::Color(150, 150, 150));
			mv.translate({ 100, 10 });
			jngl::setColor(255, 255, 255); // should be ignored
			jngl::drawRect(mv, { 120, 40 }, jngl::Color(80, 80, 80));
			mv.translate({ 100, 10 });
			jngl::drawRect(mv, { 120, 40 }, jngl::Color(0, 0, 0));
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░░░░░░░        ▒
▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▓
▒▒▒▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓███████████
▒░░░░░░░░░░▓▓▓▓▓▓▓▓▓▓███████████
▒          ░░░░░░░░░░███████████
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒███████████
)")));
		}
		for (double scaleFactor : { 1, 2 }) {
			Fixture f(scaleFactor);
			jngl::drawSquare(jngl::modelview().translate({ -100, 0 }).scale(42), 0x111111ff_rgba);
			jngl::drawLine(jngl::modelview().translate({ -20, -20 }), jngl::Vec2(0, 38), 5.f, 0x00ff00ff_rgba);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
			jngl::setLineWidth(7);
			jngl::drawLine(jngl::Vec2(15, -20), jngl::Vec2(19, 17));
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
			jngl::drawCircle(jngl::modelview().translate({ 100, -0.1 }).scale(42.05, 11.9), 0xff0000ff_rgba);
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒   ▒▒▒▒         ░             ▒
▒   ████     ░░  ▓     ░▒▒▒▒░  ▒
▒   ████     ░░  ▓    ▓▓▓▓▓▓▓▓ ▒
▒   ████     ░░  ▒░    ░▒▒▒▒░  ▒
▒   ▒▒▒▒                       ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
	};
	"triangle"_test = [] {
		for (double scaleFactor : { 0.6, 1.5 }) {
			Fixture f(scaleFactor);
			auto mv = jngl::modelview();
			jngl::setColor(0x999999ff_rgba);
			jngl::drawTriangle(jngl::Vec2(-130, 16), jngl::Vec2(-70, 10), jngl::Vec2(-80, -24));
			jngl::drawTriangle(mv.translate({50, -4}).scale(40, 18), 0x000000ff_rgba);
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒      ░            ░░         ▒
▒     ░▒░          ▒██▒        ▒
▒    ░▒▒░         ▓████▓       ▒
▒   ░░░░░                      ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
	};
	"ellipse"_test = [] {
		for (double scaleFactor : { 1, 2 }) {
			Fixture f(scaleFactor);
			auto mv = jngl::modelview();
			// Draw an ellipse centered near the left side
			jngl::drawEllipse(mv.translate({ -120, -27 }), 70, 20);
			expect(eq(f.getAsciiArt(), std::string(R"(
███████████▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
██████████▒                    ▒
▓▓▓▓▓▓▓▒░                      ▒
▒                              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
	};
};
} // namespace
