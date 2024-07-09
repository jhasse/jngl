// Copyright 2018-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <boost/ut.hpp>
#include <cassert>

int main(int argc, char** argv) {
	assert(argc <= 2);
	if (argc == 2) {
		boost::ut::cfg<boost::ut::override> = { .filter = argv[1] };
	}
	return static_cast<int>(boost::ut::cfg<>.run()); // explicitly run registered test suites
}
