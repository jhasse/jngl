// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Include this file only once, as it defines the main function
/// \file

#pragma once

#include "App.hpp"
#include "main.hpp"
#include "screen.hpp"
#include "work.hpp"

#include <cmath>

namespace jngl {
class Work;

struct AppParameters {
	/// Display name of the application which will be used in the window title for example
	std::string displayName;

	/// Size of the canvas in screen pixels, see jngl::getScreenSize()
	jngl::Vec2 screenSize;
};

} // namespace jngl

/// Implement this function and return a factory function which creates the first jngl::Work
std::function<std::shared_ptr<jngl::Work>()> jnglInit(jngl::AppParameters&);

JNGL_MAIN_BEGIN {
	jngl::AppParameters params;
	auto workFactory = jnglInit(params);
	jngl::App app(params.displayName);
	jngl::showWindow(params.displayName, std::lround(params.screenSize.x * jngl::getScaleFactor()),
	                 std::lround(params.screenSize.y * jngl::getScaleFactor()));
	jngl::setWork(workFactory());
	app.mainLoop();
}
JNGL_MAIN_END
