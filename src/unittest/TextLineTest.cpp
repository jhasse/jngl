// Copyright 2022-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/TextLine.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/ut.hpp>
#include <cmath>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT
	"TextLineTest"_test = [] {
		// Test with two rather big scale factors to avoid rounding errors:
		for (double scaleFactor : { 6, 8 }) {
			Fixture f(scaleFactor);
			jngl::Font font("../data/Arial.ttf", 14);
			jngl::TextLine text(font, "test string");
			expect(approx(text.getWidth(), 80.7, 0.2));
			expect(std::lround(text.getHeight()) == 22_l);
			text.setCenter(-80, -10);
			expect(approx(text.getX(), -120.35, 0.2));
			text.draw();
			const std::string screenshotCentered = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒   ▒░░░░░░░                   ▒
▒          ░                   ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
			expect(eq(f.getAsciiArt(), screenshotCentered));
			text.draw(jngl::modelview());
			expect(eq(f.getAsciiArt(), screenshotCentered));
		}
	};
};
} // namespace
