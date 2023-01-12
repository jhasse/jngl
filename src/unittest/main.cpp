// Copyright 2018-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <boost/ut.hpp>

int main() {
	return boost::ut::cfg<>.run(); // explicitly run registered test suites
}
