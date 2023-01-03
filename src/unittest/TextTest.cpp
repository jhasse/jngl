// Copyright 2018-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/text.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/ut.hpp>
#include <string_view>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut;

	"CharacterTest"_test = [] {
		Fixture f(2);
		jngl::Font font("../data/Arial.ttf", 40);
		jngl::Text t("m ö o ß");
		t.setFont(font);
		t.setPos(-110, -20);
		t.draw();
		const auto output = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒           ░       ░▒         ▒
▒    ░░░░  ░▒   ░░  ▒░░        ▒
▒    ▒░▒▒  ▒ ▒ ░░▒  ▒▒         ▒
▒    ▒ ▒░  ▒ ▒ ░░▒  ▒ ▒        ▒
▒    ░ ░░  ░▒░  ▒░  ░░░        ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
		expect(eq(f.getAsciiArt(), output));
		font.print("m ö o ß", -110, -20);
		expect(eq(f.getAsciiArt(), output));
		font.print("m ö o ß", jngl::Vec2(-110, -20));
		expect(eq(f.getAsciiArt(), output));
	};

	"TextTest"_test = [] {
		// Test with two rather big scale factors to avoid rounding errors:
		for (double scaleFactor : { 6, 8 }) {
			Fixture f(scaleFactor);
			jngl::setFont("../data/Arial.ttf");
			jngl::Text text("test string\nline 2");
			expect(approx(text.getWidth(), 69.4, 0.1));
			expect(std::lround(text.getHeight()) == 38_i);
			text.setCenter(-10, -10);
			expect(approx(text.getX(), -44.7, 0.1));
			text.draw();
			const std::string screenshotCentered = R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒          ░░▒░░░░░            ▒
▒                              ▒
▒           ░░                 ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)";
			expect(eq(f.getAsciiArt(), screenshotCentered));

			text.setPos(-10, -10);
			expect(std::lround(text.getX()) == -10_i);
			text.draw();
			expect(eq(f.getAsciiArt(), std::string_view(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒                              ▒
▒                  ░           ▒
▒              ░░░░░░░         ▒
▒              ░  ░            ▒
▒              ░░ ░            ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));

			text.setCenter(-10, -10); // restore previous state
			text.draw();
			expect(approx(text.getX(), -44.7, 0.1));
			expect(eq(f.getAsciiArt(), screenshotCentered));

			text.setAlign(jngl::Alignment::CENTER);
			expect(approx(text.getX(), -44.7, 0.1));
			text.draw(); // the second line should now be centered below the first
			expect(eq(f.getAsciiArt(), std::string_view(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒          ░░▒░░░░░            ▒
▒               ░              ▒
▒            ░░░░              ▒
▒                              ▒
▒                              ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));

			expect(std::lround(jngl::getTextWidth("foo")) == 22_i);
			expect(std::lround(jngl::getTextWidth("foo\nfoobar\nbar")) == 45_i);
		}
	};
};
}
