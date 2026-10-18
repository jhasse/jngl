// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

/// Test for jngl::setFrameLimiter(false): every frame calls step() exactly once, and the main loop
/// doesn't sleep, so that it runs faster than getStepsPerSecond()

#include <jngl.hpp>
#include <jngl/init.hpp>

struct MyScene : jngl::Scene {
	MyScene() {
		jngl::setVerticalSync(false);
		jngl::setFrameLimiter(false);
	}
	void step() override {
		if (steps != draws) {
			jngl::error("{} steps in {} frames, rather than one per frame", steps + 1, draws);
			std::abort();
		}
		if (++steps == 1) {
			started = jngl::getTime();
		}
		// As many steps as there are in 10 seconds, in less than half of that
		if (steps == 10 * jngl::getStepsPerSecond()) {
			const double seconds = jngl::getTime() - started;
			if (seconds > 5.) {
				jngl::error("{} steps took {} seconds, held back by the frame limiter", steps,
				            seconds);
				std::abort();
			}
			jngl::forceQuit(0);
		}
	}
	void draw() const override {
		++draws;
	}
	unsigned int steps = 0;
	mutable unsigned int draws = 0;
	double started = 0;
};

jngl::AppParameters jnglInit() {
	jngl::AppParameters params;
	params.start = []() { return std::make_shared<MyScene>(); };
	return params;
}
