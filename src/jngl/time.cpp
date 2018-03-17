// Copyright 2007-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "time.hpp"

#include <chrono>
#include <thread>

namespace jngl {
	using namespace std::chrono;

	double getTime() {
		static auto start = steady_clock::now();
		return duration_cast<duration<double>>(steady_clock::now() - start).count();
	}
	void sleep(int ms) {
		std::this_thread::sleep_for(milliseconds(ms));
	}
}
