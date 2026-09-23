// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "Vec2.hpp"

namespace jngl {

/// Size of one screen pixel in actual pixels
double getScaleFactor();

/// Overwrite the size of one screen pixel
///
/// Normally you wouldn't use this, but set AppParameters::screenSize to a lower value instead.
void setScaleFactor(double);

/// jngl::getScreenSize().x
double getScreenWidth();

/// jngl::getScreenSize().y
double getScreenHeight();

/// Returns the size of the useable draw area (excluding letter-boxing) in screen pixels
///
/// This is always the actual canvas, i.e. its size in actual pixels divided by
/// jngl::getScaleFactor(). So it takes AppParameters::scaleFactor into account, as well as a
/// window whose shape AppParameters::minAspectRatio and maxAspectRatio let differ from
/// AppParameters::screenSize. Where it matches AppParameters::screenSize, that is returned
/// exactly, without rounding errors. It doesn't change once the window has been created.
///
/// \throws std::runtime_error if the window hasn't been created yet
Vec2 getScreenSize();

} // namespace jngl
