// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Test that SIGINT (Ctrl+C) calls jngl::forceQuit(130) instead of onQuitEvent / onPauseEvent

#include <csignal>
#include <jngl.hpp>
#include <jngl/init.hpp>

struct MyScene : jngl::Scene {
	void draw() const override {
	}
	int frames = 0;
	void step() override {
		if (++frames > 10) {
			jngl::error("SIGINT did not quit the main loop");
			std::abort();
		}
		std::raise(SIGINT);
	}
	void onQuitEvent() override {
		jngl::error("onQuitEvent should not be called on SIGINT");
		std::abort();
	}
	void onPauseEvent() override {
		jngl::error("onPauseEvent should not be called on SIGINT");
		std::abort();
	}
};

jngl::AppParameters jnglInit() {
	jngl::AppParameters params;
	params.start = []() { return std::make_shared<MyScene>(); };
	return params;
}
