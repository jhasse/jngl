// Copyright 2020-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Include this file only once, as it defines the main function
/// \file

#pragma once

#include "App.hpp"
#include "main.hpp"
#include "screen.hpp"
#include "work.hpp"

#include <boost/math/special_functions/round.hpp>
#include <cmath>
#include <optional>

namespace jngl {
class Work;

/// Parameters used to initialize the main window
struct AppParameters {
	/// Display name of the application which will be used in the window title for example
	std::string displayName;

	/// Size of the canvas in screen pixels, see jngl::getScreenSize()
	///
	/// If not specified JNGL will create a fullscreen Window with the maximum of space available.
	std::optional<jngl::Vec2> screenSize;

	/// Activates pixel-perfect magnifying of textures (nearest-neighbor interpolation)
	bool pixelArt = false;
};

} // namespace jngl

/// Implement this function and return a factory function which creates the first jngl::Work
std::function<std::shared_ptr<jngl::Work>()> jnglInit(jngl::AppParameters&);

#if !defined(__APPLE__) || !TARGET_OS_IPHONE // iOS
JNGL_MAIN_BEGIN { // NOLINT
	jngl::AppParameters params;
	auto workFactory = jnglInit(params);
	jngl::App app(params.displayName);
	app.setPixelArt(params.pixelArt);
	bool fullscreen = false;
#ifdef NDEBUG
	fullscreen = true;
#endif
	std::pair<int, int> minAspectRatio{ 1, 3 };
	std::pair<int, int> maxAspectRatio{ 3, 1 };
	if (!params.screenSize) {
		params.screenSize = { double(800), double(600) };
		fullscreen = true;
	}
	if (fullscreen) {
		const jngl::Vec2 desktopSize{ double(800),
			                          double(600) };
		jngl::setScaleFactor(
		    std::min(desktopSize.x / params.screenSize->x, desktopSize.y / params.screenSize->y));
		maxAspectRatio = minAspectRatio =
		    std::pair<int, int>(params.screenSize->x, params.screenSize->y);
	} else {
		// Make window as big as possible
		const int scaleFactor = std::min((800 - 50) / params.screenSize->x,
		                                 (600 - 50) / params.screenSize->y);
		jngl::setScaleFactor(std::max(1, scaleFactor));
	}
	jngl::showWindow(
	    params.displayName,
	    fullscreen ? 800
	               : boost::math::iround(params.screenSize->x * jngl::getScaleFactor()),
	    fullscreen ? 600
	               : boost::math::iround(params.screenSize->y * jngl::getScaleFactor()),
	    fullscreen, minAspectRatio, maxAspectRatio);
	jngl::setWork(workFactory());
	app.mainLoop();
}
JNGL_MAIN_END
#endif
