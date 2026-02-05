// Copyright 2007-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <chrono>
#include <thread>

namespace jngl {

// for getTime() see ../timing/realtime.cpp

void sleep(const int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

} // namespace jngl
