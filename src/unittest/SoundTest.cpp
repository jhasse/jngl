// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include <boost/ut.hpp>
#include <jngl/sound.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT
	"Sound"_test = [] {
		expect(!jngl::isPlaying("../data/test.ogg"));
		jngl::play("../data/test.ogg");
		expect(jngl::isPlaying("../data/test.ogg"));
		jngl::stop("../data/test.ogg");
		expect(!jngl::isPlaying("../data/test.ogg"));
		expect(throws<std::runtime_error>([] { jngl::play("nonexistent.ogg"); }));
		expect(throws<std::runtime_error>([] { jngl::stop("nonexistent.ogg"); }));
		expect(throws<std::runtime_error>([] { jngl::isPlaying("nonexistent.ogg"); }));
	};
};
} // namespace
