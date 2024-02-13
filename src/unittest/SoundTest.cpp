// Copyright 2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/sound.hpp"
#include "Fixture.hpp"

#include <jngl.hpp>

#include <boost/ut.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT
	"Sound"_test = [] {
        Fixture f(1);
		jngl::play("../data/test.ogg");
        expect(jngl::isPlaying("../data/test.ogg"));
		jngl::stop("../data/test.ogg");
        expect(!jngl::isPlaying("../data/test.ogg"));
	};
};
} // namespace
