// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>

class Fixture {
public:
	Fixture(double scaleFactor);
	~Fixture();

	std::string getAsciiArt() const;

private:
	void reset() const;

	std::string emptyAsciiArt;
};
