// Copyright 2012-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "dll.hpp"
#include "Vec2.hpp"

namespace jngl {

/// Size of one screen pixel in actual pixels
double JNGLDLL_API getScaleFactor();

void JNGLDLL_API setScaleFactor(double);

/// jngl::getScreenSize().x
double JNGLDLL_API getScreenWidth();

/// jngl::getScreenSize().y
double JNGLDLL_API getScreenHeight();

/// Returns the size of the useable draw area (excluding letter-boxing) in screen pixels
Vec2 JNGLDLL_API getScreenSize();

enum DeviceOrientation { Portrait, PortraitUpsideDown, LandscapeLeft, LandscapeRight };

void JNGLDLL_API setDeviceOrientationSupported(DeviceOrientation, bool enabled);

bool JNGLDLL_API getDeviceOrientationSupported(DeviceOrientation);

} // namespace jngl
