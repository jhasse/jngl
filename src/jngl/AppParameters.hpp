// Copyright 2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::AppParameters struct
/// \file
#pragma once

#include "Vec2.hpp"

#include <optional>

namespace jngl {

/// Parameters used to initialize the main window
struct AppParameters {
	/// Display name of the application which will be used in the window title for example
	std::string displayName;

	/// Size of the canvas in screen pixels, see jngl::getScreenSize()
	///
	/// If not specified JNGL will create a fullscreen Window with the maximum of space available.
	std::optional<jngl::Vec2> screenSize;

	std::optional<std::pair<int, int>> minAspectRatio;
	std::optional<std::pair<int, int>> maxAspectRatio;

	/// Activates pixel-perfect magnifying of textures (nearest-neighbor interpolation)
	bool pixelArt = false;
};

} // namespace jngl
