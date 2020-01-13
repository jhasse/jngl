// Copyright 2007-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <chrono>
#include <thread>

namespace jngl {

double getTime() {
	static auto start = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::duration<double>>(
	           std::chrono::steady_clock::now() - start)
	    .count();
}

void sleep(const int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

} // namespace jngl
