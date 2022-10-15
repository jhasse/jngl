// Copyright 2020-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Include this file only once, as it defines the main function
/// \file

#pragma once

#include "App.hpp"
#include "AppParameters.hpp"
#include "main.hpp"
#include "message.hpp"
#include "screen.hpp"
#include "work.hpp"

#include <cmath>

#if defined(__has_include) && __has_include("filesystem")
#include <filesystem>
#endif

namespace jngl {
class Work;
} // namespace jngl

/// Implement this function and set AppParameters::start
///
/// Usually you'd want to do this in a file called e.g. `main.cpp`:
/// \code
/// #include "MyGame.hpp" // class that derives from jngl::Work
///
/// #include <jngl/init.hpp>
///
/// jngl::AppParameters jnglInit() {
///     jngl::AppParameters params;
///     params.displayName = "My Game";
///     params.screenSize = { 1920, 1080 };
/// 
///     params.start = []() {
///         return std::make_shared<MyGame>();
///     };
///     return params;
/// }
/// \endcode
jngl::AppParameters jnglInit();

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
			if (err) {
				std::filesystem::current_path("../../../data", err); // move out of out\build\x64-Debug
				if (err) {
					std::filesystem::current_path(jngl::getBinaryPath() + "data", err);
				}
			}
		}
	}
#endif
	jngl::AppParameters params = jnglInit();
	auto& app = jngl::App::instance();
	app.setDisplayName(params.displayName);
	app.setPixelArt(params.pixelArt);
	bool fullscreen = false;
#if (!defined(__EMSCRIPTEN__) && defined(NDEBUG)) || defined(__ANDROID__)
	fullscreen = true;
#endif
	std::pair<int, int> minAspectRatio{ 1, 3 };
	std::pair<int, int> maxAspectRatio{ 3, 1 };
	if (params.screenSize) {
		maxAspectRatio = minAspectRatio = std::pair<int, int>(std::lround(params.screenSize->x),
		                                                      std::lround(params.screenSize->y));
	} else {
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
		}
	} else {
		// Make window as big as possible
		const double scaleFactor = std::min((jngl::getDesktopWidth() - 99) / params.screenSize->x,
		                                    (jngl::getDesktopHeight() - 99) / params.screenSize->y);
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
	                 fullscreen, params.minAspectRatio ? *params.minAspectRatio : minAspectRatio,
	                 params.maxAspectRatio ? *params.maxAspectRatio : maxAspectRatio);
	jngl::setWork(params.start());
	app.mainLoop();
}
JNGL_MAIN_END
#endif
