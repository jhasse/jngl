// Copyright 2019-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Color.hpp"
#include "../jngl/Rgb.hpp"
#include "../jngl/Rgba.hpp"

#include <boost/ut.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT
	"Color"_test = [] {
		const jngl::Color a{ 0x68da4f_rgb };
		const auto b = jngl::Color(0x68, 0xda, 0x4f);
		const auto c = jngl::Color(104, 218, 79);
		expect(eq(a.getRed(), b.getRed()));
		expect(eq(c.getRed(), b.getRed()));
		expect(eq(a.getGreen(), b.getGreen()));
		expect(eq(c.getGreen(), b.getGreen()));
		expect(eq(a.getBlue(), b.getBlue()));
		expect(eq(c.getBlue(), b.getBlue()));
	};
	"Rgb"_test = [] {
		const jngl::Rgb a = 0x68da4f_rgb;
		const auto b = jngl::Rgb::u8(0x68, 0xda, 0x4f);
		const auto c = jngl::Rgb::u8(104, 218, 79);
		expect(eq(a.getRed(), b.getRed()));
		expect(eq(c.getRed(), b.getRed()));
		expect(eq(a.getGreen(), b.getGreen()));
		expect(eq(c.getGreen(), b.getGreen()));
		expect(eq(a.getBlue(), b.getBlue()));
		expect(eq(c.getBlue(), b.getBlue()));
	};
	"Rgba"_test = [] {
		const jngl::Rgba a = 0x68da4f4f_rgba;
		const auto b = jngl::Rgba::u8(0x68, 0xda, 0x4f, 0x4f);
		const auto c = "#68da4f4f"_rgba;
		expect(eq(a.getRed(), b.getRed()));
		expect(eq(c.getRed(), b.getRed()));
		expect(eq(a.getGreen(), b.getGreen()));
		expect(eq(c.getGreen(), b.getGreen()));
		expect(eq(a.getBlue(), b.getBlue()));
		expect(eq(c.getBlue(), b.getBlue()));
		expect(eq(a.getAlpha(), b.getAlpha()));
		expect(eq(c.getAlpha(), b.getAlpha()));
	};
};
} // namespace
