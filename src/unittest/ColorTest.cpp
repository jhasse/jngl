// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Color.hpp"

#include <boost/ut.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut;
	"Color"_test = [] {
		const auto a = 0x68da4f_rgb;
		const auto b = jngl::Color(0x68, 0xda, 0x4f);
		const auto c = jngl::Color(104, 218, 79);
		expect(eq(a.getRed(), b.getRed()));
		expect(eq(c.getRed(), b.getRed()));
		expect(eq(a.getGreen(), b.getGreen()));
		expect(eq(c.getGreen(), b.getGreen()));
		expect(eq(a.getBlue(), b.getBlue()));
		expect(eq(c.getBlue(), b.getBlue()));
	};
};
} // namespace
