// Copyright 2020-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Include this file only once, as it defines the main function
/// \file

#pragma once

#include "App.hpp"
#include "main.hpp"
#include "screen.hpp"
#include "work.hpp"

#include <cmath>
#include <optional>

#if defined(__has_include) && __has_include("filesystem")
#include <filesystem>
#endif

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
JNGL_MAIN_BEGIN {                            // NOLINT
#if !defined(ANDROID) && defined(__has_include) && __has_include(<filesystem>) && \
    (!defined(TARGET_OS_IOS) || TARGET_OS_IOS == 0)
	std::error_code err;
	std::filesystem::current_path("data", err);
	if (err) {
		std::filesystem::current_path("../data", err); // move out of build/bin folder
		if (err) {
			std::filesystem::current_path("../../data", err); // move out of build/Debug folder
		}
	}
#endif
	jngl::AppParameters params;
	auto workFactory = jnglInit(params);
	auto& app = jngl::App::instance();
	app.setDisplayName(params.displayName);
	app.setPixelArt(params.pixelArt);
	bool fullscreen = false;
#if defined(NDEBUG) || defined(__ANDROID__)
	fullscreen = true;
#endif
	std::pair<int, int> minAspectRatio{ 1, 3 };
	std::pair<int, int> maxAspectRatio{ 3, 1 };
	if (!params.screenSize) {
		params.screenSize = { double(jngl::getDesktopWidth()), double(jngl::getDesktopHeight()) };
		fullscreen = true;
	}
	if (fullscreen) {
		const jngl::Vec2 desktopSize{ double(jngl::getDesktopWidth()),
			                          double(jngl::getDesktopHeight()) };
		if (desktopSize.x > 0 &&
		    desktopSize.y > 0) { // desktop size isn't available on some platforms (e.g. Android)
			jngl::setScaleFactor(std::min(desktopSize.x / params.screenSize->x,
			                              desktopSize.y / params.screenSize->y));
			maxAspectRatio = minAspectRatio = std::pair<int, int>(
			    std::lround(params.screenSize->x), std::lround(params.screenSize->y));
		}
	} else {
		// Make window as big as possible
		const double scaleFactor = std::min((jngl::getDesktopWidth() - 50) / params.screenSize->x,
		                                    (jngl::getDesktopHeight() - 50) / params.screenSize->y);
		if (scaleFactor > 1) {
			jngl::setScaleFactor(std::floor(scaleFactor));
		} else {
			jngl::setScaleFactor(scaleFactor);
		}
	}
	jngl::showWindow(params.displayName,
	                 fullscreen ? jngl::getDesktopWidth()
	                            : int(std::lround(params.screenSize->x * jngl::getScaleFactor())),
	                 fullscreen ? jngl::getDesktopHeight()
	                            : int(std::lround(params.screenSize->y * jngl::getScaleFactor())),
	                 fullscreen, minAspectRatio, maxAspectRatio);
	jngl::setWork(workFactory());
	app.mainLoop();
}
JNGL_MAIN_END
#endif
