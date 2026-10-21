// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::GpuTimer class
/// @file
#pragma once

#include <chrono>
#include <memory>
#include <optional>

namespace jngl {

/// Measures how long the GPU takes for what's drawn between start() and stop(), e.g. a frame,
/// without waiting for it to finish: a measurement comes in a few frames later, see poll().
///
/// Needs OpenGL 3.3's timer queries, which OpenGL ES and WebGL don't have. Without them, see
/// isSupported(), start() and stop() do nothing and poll() never returns anything.
///
/// Example:
/// \code
/// timer.start();
/// scene.draw();
/// timer.stop();
/// while (const auto duration = timer.poll()) {
///     // a frame some time ago took *duration on the GPU
/// }
/// \endcode
class GpuTimer {
public:
	GpuTimer();
	~GpuTimer();
	GpuTimer(const GpuTimer&) = delete;
	GpuTimer& operator=(const GpuTimer&) = delete;
	GpuTimer(GpuTimer&&) noexcept;
	GpuTimer& operator=(GpuTimer&&) noexcept;

	/// Whether the GPU's time can be measured here
	static bool isSupported();

	/// Starts measuring what's drawn from now on. Only one GpuTimer can measure at a time, and only
	/// once until stop().
	void start();

	/// Stops measuring, for poll() to return how long the GPU took once that's known
	void stop();

	/// The oldest measurement that has come in since the last call, in the order of stop(), or
	/// nullopt if there's none (yet)
	std::optional<std::chrono::nanoseconds> poll();

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace jngl
