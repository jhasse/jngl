// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "Vec2.hpp"

namespace jngl {

/// Size of one screen pixel in actual pixels
double getScaleFactor();

void setScaleFactor(double);

/// jngl::getScreenSize().x
double getScreenWidth();

/// jngl::getScreenSize().y
double getScreenHeight();

/// Returns the size of the useable draw area (excluding letter-boxing) in screen pixels
Vec2 getScreenSize();

} // namespace jngl
