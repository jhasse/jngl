// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>
#include <vector>

namespace jngl {

void clearBackgroundColor();

bool Init(int width, int height, int canvasWidth, int canvasHeight);

extern std::string pathPrefix;
extern std::string configPath;
extern std::vector<std::string> args;

} // namespace jngl
