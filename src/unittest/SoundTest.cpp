// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include <boost/ut.hpp>
#include <jngl/sound.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT
	"Sound"_test = [] {
		jngl::play("../data/test.ogg");
		expect(jngl::isPlaying("../data/test.ogg"));
		jngl::stop("../data/test.ogg");
		expect(!jngl::isPlaying("../data/test.ogg"));
	};
};
} // namespace
