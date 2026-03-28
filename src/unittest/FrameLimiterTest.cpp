// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../timing/FrameLimiter.hpp"
#include "../log.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cassert>

struct VsyncTiming {
	explicit VsyncTiming(double hz) : microsecondsPerFrame(static_cast<uint64_t>(1e6 / hz)) {
	}

	double getTime() const {
		return static_cast<double>(time_us) / 1e6;
	}

	void sleep(int64_t microseconds) {
		assert(microseconds >= 0);
		time_us += microseconds;
		jngl::internal::trace("Sleep for {}ms", microseconds / 1000);
	}

	void swapBuffers() {
		auto timeSinceLastSwap = time_us - lastSwapTime_us;
		lastSwapTime_us = time_us;
		if (timeSinceLastSwap >= microsecondsPerFrame) {
			return;
		}
		time_us += microsecondsPerFrame - timeSinceLastSwap; // vsync will make sure that we are at
		                                                     // the beginning of the next frame
		jngl::internal::trace("Swap buffers, wait for {}ms until next frame",
		                      (microsecondsPerFrame - timeSinceLastSwap) / 1000);
	}

	uint64_t time_us = 0;

private:
	uint64_t microsecondsPerFrame;
	uint64_t lastSwapTime_us = 0;
};

TEST_CASE("FrameLimiter") {
	// We use a custom getTime and sleep function to make the test deterministic and not depend
	// on the actual time.
	uint64_t time_us = 0;
	auto getTime = [&time_us]() { return static_cast<double>(time_us) / 1e6; };
	auto sleep = [&time_us](int64_t us) {
		assert(us >= 0);
		time_us += us;
	};

	{ // check that when step and draw take no time we sleep and stay at 60 FPS:
		uint64_t totalSteps = 0;
		jngl::FrameLimiter limiter{ 1. / 60., getTime };
		for (int i = 0; i < 100000; ++i) {
			auto numberOfSteps = limiter.check();
			REQUIRE(numberOfSteps == 1U);
			totalSteps += numberOfSteps;
			time_us += 2 * 1e3; // draw takes 2 ms, step takes 0 ms
			const auto oldTime = time_us;
			limiter.sleepIfNeeded(sleep);
			const auto timeSlept = time_us - oldTime;
			REQUIRE(timeSlept <= 16'667u); // we should never sleep more than 16.67 ms
		}
		auto averageSPS = totalSteps * 1'000'000 / time_us;
		REQUIRE(averageSPS >= 59u);
		REQUIRE(averageSPS <= 61u);
	}
	{
		time_us = 0;
		jngl::FrameLimiter limiter{ 1. / 60., getTime };
		bool first = true;
		uint64_t totalSteps = 0;
		for (int i = 0; i < 100000; ++i) {
			auto numberOfSteps = limiter.check();
			if (first) {
				REQUIRE(numberOfSteps == 1U);
				first = false;
			} else {
				REQUIRE(numberOfSteps == 2U);
			}
			totalSteps += numberOfSteps;
			time_us += 20 * 1e3; // draw takes 20 ms, step takes 0 ms, so we are already too
			                     // slow, so we should not sleep at all.
			limiter.sleepIfNeeded(sleep);
		}
		auto averageSPS = totalSteps * 1'000'000 / time_us;
		REQUIRE(averageSPS >= 59u);
		REQUIRE(averageSPS <= 61u);
	}
}

TEST_CASE("FrameLimiter_Vsync") {
	VsyncTiming timing{ 30 }; // simulate a display with 30 Hz refresh rate, so we can check if
	                          // we are correctly detecting it and not trying to do 60 FPS.
	jngl::FrameLimiter limiter{ 1. / 60., [&timing]() { return timing.getTime(); } };
	uint64_t totalSteps = 0;
	int frameRateChanges = 0;
	uint32_t lastNumberOfSteps = 1;
	for (int i = 0; i < 9000; ++i) {
		auto numberOfSteps = limiter.check();
		if (numberOfSteps != lastNumberOfSteps) {
			++frameRateChanges;
			lastNumberOfSteps = numberOfSteps;
		}
		totalSteps += numberOfSteps;
		timing.time_us += 2 * 1'000 + numberOfSteps * 3 * 1'000; // draw takes 2 ms, step takes 3 ms
		timing.swapBuffers();
		limiter.sleepIfNeeded([&timing](int64_t us) { timing.sleep(us); });
	}
	auto averageSPS = totalSteps * 1'000'000 / timing.time_us;
	REQUIRE(averageSPS >= 59u);
	REQUIRE(averageSPS <= 61u);
	REQUIRE(frameRateChanges <
	        20); // we should quickly detect the refresh rate and then not change it anymore
}
