// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../jngl/Channel.hpp"
#include "../jngl/sound.hpp"

#include <boost/ut.hpp>

namespace {
boost::ut::suite _ = [] {
	using namespace boost::ut; // NOLINT
	"Channel"_test = [] {
		jngl::Channel channel;
		expect(!channel.isPlaying("../data/test.ogg"));
		expect(!jngl::Channel::main().isPlaying("../data/test.ogg"));
		jngl::play("../data/test.ogg");
		expect(!channel.isPlaying("../data/test.ogg"));
		expect(jngl::Channel::main().isPlaying("../data/test.ogg"));
		channel.play("../data/test.ogg");
		expect(channel.isPlaying("../data/test.ogg"));
		expect(jngl::Channel::main().isPlaying("../data/test.ogg"));
		jngl::Channel::main().stop("../data/test.ogg");
		expect(channel.isPlaying("../data/test.ogg"));
		expect(!jngl::Channel::main().isPlaying("../data/test.ogg"));
		channel.stop("../data/test.ogg");
		expect(!channel.isPlaying("../data/test.ogg"));
		expect(!jngl::Channel::main().isPlaying("../data/test.ogg"));
		expect(throws<std::runtime_error>([&] { channel.play("nonexistent.ogg"); }));
		expect(throws<std::runtime_error>([&] { channel.stop("nonexistent.ogg"); }));
		expect(throws<std::runtime_error>([&] { channel.isPlaying("nonexistent.ogg"); }));
	};
};
} // namespace
