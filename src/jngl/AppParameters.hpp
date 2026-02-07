// Copyright 2022-2026 Jan Niklas Hasse <jhasse@bixense.com>
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

	/// If set, can be used to control whether to run in fullscreen or windowed mode on supported
	/// platforms. If not set, it will run fullscreen when NDEBUG is defined (i.e. in Release mode).
	std::optional<bool> fullscreen;

	std::optional<std::pair<int, int>> minAspectRatio;
	std::optional<std::pair<int, int>> maxAspectRatio;

	/// If set and JNGL_STEAMWORKS has been passed to CMake, JNGL will ensure the app runs through
	/// Steam and initialize the SteamAPI
	std::optional<uint32_t> steamAppId;

	/// Activates pixel-perfect magnifying of textures (nearest-neighbor interpolation)
	bool pixelArt = false;

	/// This function can be set to make adjustments to the scale factor used by
	/// jngl::getScaleFactor() after the window has been created. It gets the actual window
	/// dimensions in actual pixels as parameters (width, height) and must return the desired new
	/// scale factor.
	///
	/// For example if you don't want to force an aspect ratio, but want to have a consistent
	/// scaling based on a 1920x1080 screen, you could use:
	/// \code{.cpp}
	/// params.scaleFactor = [](int width, int height) {
	///     const double windowSqr = width * height;
	///     const double desiredSqr = 1920 * 1080;
	///     return sqrt(windowSqr / desiredSqr);
	/// };
	/// \endcode
	std::function<double(int, int)> scaleFactor;
};

} // namespace jngl
