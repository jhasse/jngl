// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../jngl/time.hpp"

#include <cstdint>
#include <functional>

namespace jngl {

class FrameLimiter {
public:
	explicit FrameLimiter(double timePerStep);

	/// Return how many times step() should be called before the next draw()
	unsigned int check();

	void sleepIfNeeded(const std::function<void(int64_t)>& sleepMicroseconds);

private:
	/*const*/ double timePerStep;
	/*const*/ unsigned int maxStepsPerFrame = 3;

	/// How many step() calls per draw() call
	unsigned int stepsPerFrame = 1;

	double lastCheckTime = getTime();
	double sleepCorrectionFactor = 1;
	unsigned int stepsSinceLastCheck = 0;

	/// When VSYNC is active we will try to find out to what FPS/Hz the display is limiting us
	double maxFPS = 300;

	/// How often the frame limiter has run
	unsigned int numberOfChecks = 0;

	double timeSleptSinceLastCheck = 0;
	unsigned int previousStepsPerFrame = 1;
	double sleepPerFrame = 0; // in seconds
	unsigned int numberOfSleeps = 0;
};

} // namespace jngl
