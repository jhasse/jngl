// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/OutlinedFont.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/ut.hpp>
#include <cmath>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut;
	"OutlinedFont"_test = [] {
		for (double scaleFactor : { 1, 2 }) {
			Fixture f(scaleFactor);
			jngl::OutlinedFont font("../data/Arial.ttf", 70, 5);
			font.print(jngl::modelview().translate({ -120, -40 }), "m", 0x55555500_rgba,
			           0x000000ff_rgba);
			font.print(jngl::modelview().translate({ 0, -40 }), ".", 0x0000000ff_rgba,
						0x00000000_rgba);
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒   ░▒▒▒░▒▒                    ▒
▒   ▒█▒██▒█░                   ▒
▒   ▒▓ ▓▒ ▓░                   ▒
▒   ▒▒ ▓▒ ▓░                   ▒
▒   ▒▒ ▓▒ ▓░     ░             ▒
▓▒▒▒▓▓▒▓▓▒█▓▒▒▒▒▒▓▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
	};
};
} // namespace
