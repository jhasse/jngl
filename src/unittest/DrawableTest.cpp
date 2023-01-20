// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Drawable.hpp"

#include <boost/ut.hpp>

class Dummy : public jngl::Drawable {
public:
	void step() override {
	}
	void draw() const override {
	}
};

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut;
	"Drawable"_test = [] {
		Dummy d;
		expect(approx(d.getX(), 0, 1e-9));
		expect(approx(d.getY(), 0, 1e-9));
		expect(approx(d.getWidth(), 0, 1e-9));
		expect(approx(d.getHeight(), 0, 1e-9));
		expect(approx(d.getWidth(), d.getSize().x, 1e-9));
		expect(approx(d.getHeight(), d.getSize().y, 1e-9));
	};
};
} // namespace
