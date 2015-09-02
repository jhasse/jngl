/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include <vector>
#include <string>

namespace jngl {
	void clearBackgroundColor();

	bool Init(const int width, const int height);

	extern std::string pathPrefix;
	extern std::vector<std::string> args;
}
