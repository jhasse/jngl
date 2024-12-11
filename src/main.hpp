// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>
#include <vector>
#if defined(__has_include) && __has_include(<optional>)
#include <optional>
using std::optional;
#else
#include <experimental/optional>
using std::experimental::optional;
#endif

namespace jngl {

void clearBackgroundColor();

bool Init(int width, int height, int canvasWidth, int canvasHeight);
void updateViewportAndLetterboxing(int width, int height, int canvasWidth, int canvasHeight);
void updateProjection(int windowWidth, int windowHeight, int, int);

extern std::string pathPrefix;
extern optional<std::string> configPath;
extern std::vector<std::string> args;

} // namespace jngl
