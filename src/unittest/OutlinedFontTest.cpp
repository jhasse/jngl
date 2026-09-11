// Copyright 2025-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../jngl/OutlinedFont.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/ut.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT
	"OutlinedFont"_test = [] {
		for (double scaleFactor : { 1.5, 3. }) {
			Fixture f(scaleFactor);
			std::shared_ptr<jngl::FontInterface> fontInterface;
			{
				jngl::OutlinedFont font("../data/Arial.ttf", 60, 5);
				font.print(jngl::modelview().translate({ -120, -40 }), "m", 0x55555500_rgba,
				           0x000000ff_rgba);
				font.print(jngl::modelview().translate({ 0, -40 }), ".", 0x0000000ff_rgba,
				           0x00000000_rgba);
				fontInterface = font.bake(0x55555500_rgba, 0x000000ff_rgba);
			}
			fontInterface->print(jngl::modelview().translate({ 60, -40 }), "m");
			expect(eq(f.getAsciiArt(), std::string(R"(
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
▒   ▓██▓██░           ▓██▓██░  ▒
▒   ▓▓▒█░█▒           ▓▓▒█░█▒  ▒
▒   ▓▒░█ ▓▒           ▓▒░█ ▓▒  ▒
▒   ▓▒░█ ▓▒      ░    ▓▒░█ ▓▒  ▒
▒   ▒▒░▓ ▓░     ░░    ▒▒░▓ ▓░  ▒
▓▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▓
)")));
		}
	};
};
} // namespace
