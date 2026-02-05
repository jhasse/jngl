// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "FrameLimiter.hpp"
#include "../jngl/time.hpp"

#include <thread>

namespace jngl {

/// Sleep current thread for \a seconds
void sleepSeconds(double seconds) {
	std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

double getTime() {
	static auto start = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::duration<double>>(
	           std::chrono::steady_clock::now() - start)
	    .count();
}

} // namespace jngl
