/*
Copyright 2007-2013 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include <chrono>
#include <thread>

namespace jngl {
	using namespace std::chrono;

	double getTime() {
		static auto start = high_resolution_clock::now();
		return duration_cast<duration<double>>(high_resolution_clock::now() - start).count();
	}
	void sleep(int ms) {
		std::this_thread::sleep_for(milliseconds(ms));
	}
}
