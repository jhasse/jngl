// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "FrameLimiter.hpp"

#include "../log.hpp"

#include <algorithm>
#include <cmath>

namespace jngl {

FrameLimiter::FrameLimiter(const double timePerStep)
: timePerStep(timePerStep),
  maxStepsPerFrame(static_cast<unsigned int>(
      std::lround(1.0 / 20.0 / timePerStep)) /* Never drop below 20 FPS, instead slow down */) {
}

unsigned int FrameLimiter::check() {
	const auto currentTime = getTime();
	const auto secondsSinceLastCheck = currentTime - lastCheckTime;
	const auto targetStepsPerSecond = 1.0 / timePerStep;
	// If SPS == FPS, this would mean that we check about every second, but in the beginning we
	// want to check more often, e.g. to quickly adjust to high refresh rate monitors:
	if (stepsSinceLastCheck > targetStepsPerSecond || stepsSinceLastCheck > numberOfChecks) {
		++numberOfChecks;
		const auto actualStepsPerSecond = stepsSinceLastCheck / secondsSinceLastCheck;
		auto doableStepsPerSecond =
		    stepsSinceLastCheck / (secondsSinceLastCheck - timeSleptSinceLastCheck);
		if (previousStepsPerFrame > stepsPerFrame && actualStepsPerSecond < targetStepsPerSecond) {
			maxFPS = 0.5 * maxFPS + 0.5 * actualStepsPerSecond / stepsPerFrame;
		} else {
			maxFPS += sleepPerFrame;
		}
		previousStepsPerFrame = stepsPerFrame;
		const auto cappedOrDoable = std::min(doableStepsPerSecond, maxFPS * stepsPerFrame);

		// The sleep function is actually inaccurate (or at least less accurate than getTime),
		// se we try to find a factor to correct this:
		sleepCorrectionFactor +=
		    0.1 * // don't change it too fast
		    (sleepPerFrame * stepsSinceLastCheck / stepsPerFrame - timeSleptSinceLastCheck);
		//   ↑__________seconds we should have slept___________↑   ↑___actual seconds____↑

		// Clamp it in case of some bug:
		sleepCorrectionFactor = std::max(0.1, std::min(sleepCorrectionFactor, 2.0));

		// Round up, because if we can do 40 FPS, but need 60 SPS, we need at least 2 SPF. We
		// don't round up exactly to be a little bit "optimistic" of what we can do.
		auto newStepsPerFrame =
		    std::min({ static_cast<unsigned int>(std::max(
		                   1, static_cast<int>(0.98 + stepsPerFrame * targetStepsPerSecond /
		                                                  cappedOrDoable))),
		               stepsPerFrame * 2, maxStepsPerFrame }); // never increase too much
		// Divide doableStepsPerSecond by the previous stepsPerFrame and multiply it with
		// newStepsPerFrame so that we know what can be doable in the future and not what
		// could have been doable:
		double shouldSleepPerFrame =
		    newStepsPerFrame * // we sleep per frame, not per step
		    (timePerStep - 1.0 / (newStepsPerFrame * doableStepsPerSecond / stepsPerFrame));
		if (shouldSleepPerFrame < 0) {
			shouldSleepPerFrame = 0;
		}
		// The factor means that we quickly go down when needed, but hesitate to go up:
		sleepPerFrame += ((shouldSleepPerFrame < sleepPerFrame) ? 0.95 : 0.55) *
		                 (shouldSleepPerFrame - sleepPerFrame);

		internal::trace("SPS: {} ({} {}, should be {}); stepsPerFrame -> {}, msSleepPerFrame -> {} "
		                "* {}, slept({}): {}µs, maxFPS: {}",
		                std::lround(actualStepsPerSecond),
		                (cappedOrDoable < doableStepsPerSecond) ? "capped" : "doable",
		                std::lround(doableStepsPerSecond), std::lround(targetStepsPerSecond),
		                newStepsPerFrame, sleepPerFrame, sleepCorrectionFactor, numberOfSleeps,
		                std::lround(1e6 * timeSleptSinceLastCheck), maxFPS);

		lastCheckTime = currentTime;
		numberOfSleeps = 0;
		stepsSinceLastCheck = 0;
		timeSleptSinceLastCheck = 0;
		stepsPerFrame = newStepsPerFrame;
	}
	stepsSinceLastCheck += stepsPerFrame;
	return stepsPerFrame;
}

void FrameLimiter::sleepIfNeeded(const std::function<void(int64_t)>& sleepMicroseconds) {
	const auto start = getTime();
	const auto shouldBe = lastCheckTime + timePerStep * stepsSinceLastCheck;
	const int64_t micros =
	    std::lround((sleepPerFrame - (start - shouldBe)) * sleepCorrectionFactor * 1e6);
	if (micros > 0) {
		sleepMicroseconds(micros);
		timeSleptSinceLastCheck += jngl::getTime() - start;
		++numberOfSleeps;
	}
}

} // namespace jngl
