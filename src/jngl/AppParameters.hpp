// Copyright 2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::AppParameters struct
/// \file
#pragma once

#include "Vec2.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace jngl {

class Work;

/// Parameters used to initialize the main window
struct AppParameters {
	/// A factory function which creates the first jngl::Work
	std::function<std::shared_ptr<jngl::Work>()> start;

	/// Display name of the application which will be used in the window title for example
	std::string displayName;

	/// Size of the canvas in screen pixels, see jngl::getScreenSize()
	///
	/// If not specified JNGL will create a fullscreen Window with the maximum of space available.
	std::optional<jngl::Vec2> screenSize;

	std::optional<std::pair<int, int>> minAspectRatio;
	std::optional<std::pair<int, int>> maxAspectRatio;

	/// If set and JNGL_STEAMWORKS has been passed to CMake, JNGL will ensure the app runs through
	/// Steam and initialize the SteamAPI
	std::optional<uint32_t> steamAppId;

	/// Activates pixel-perfect magnifying of textures (nearest-neighbor interpolation)
	bool pixelArt = false;
};

} // namespace jngl
