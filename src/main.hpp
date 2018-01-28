// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <vector>
#include <string>

namespace jngl {
	void clearBackgroundColor();

	bool Init(int width, int height, int screenWidth, int screenHeight);

	extern std::string pathPrefix;
	extern std::vector<std::string> args;
}
