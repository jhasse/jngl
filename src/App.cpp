// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "App.hpp"

#include "jngl/AppParameters.hpp"
#include "jngl/ShaderProgram.hpp"
#include "jngl/debug.hpp"
#include "jngl/screen.hpp"
#include "jngl/window.hpp"
#include "jngl/work.hpp"
#include "windowptr.hpp"

#include <cmath>
#include <set>
#include <stdexcept>

namespace jngl {

/// Restarts the program via Steam if needed
void initSteamAchievements();

/// Initializes the Steamworks SDK (if JNGL_STEAMWORKS has been set via CMake)
void initSteam(uint32_t steamAppId);

App* App::self = nullptr;

struct App::Impl {
	std::string displayName;
	bool pixelArt = false;
	std::optional<uint32_t> steamAppId;
	std::set<ShaderProgram*> shaderPrograms{};
};

App::App(AppParameters params)
: impl(new Impl{ std::move(params.displayName), params.pixelArt, params.steamAppId }) {
	if (self) {
		throw std::runtime_error("You may only create one instance of jngl::App.");
	}
	self = this;
	if (impl->steamAppId) {
		jngl::initSteam(*impl->steamAppId);
	}
}

App::~App() {
	self = nullptr;
}

App& App::instance() {
	if (!self) {
		throw std::runtime_error("App hasn't been created yet.");
	}
	return *self;
}

std::string App::getDisplayName() const {
	return impl->displayName;
}

void App::setDisplayName(const std::string& displayName) {
	impl->displayName = displayName;
}

void App::mainLoop() {
	if (impl->steamAppId) {
		initSteamAchievements();
	}
	debug("Starting main loop for '");
	debug(impl->displayName);
	debugLn('\'');
	return pWindow->mainLoop();
}

bool App::isPixelArt() {
	return self ? self->impl->pixelArt : false;
}

void App::setPixelArt(const bool pixelArt) {
	impl->pixelArt = pixelArt;
}

void App::registerShaderProgram(ShaderProgram* shaderProgram) {
	impl->shaderPrograms.insert(shaderProgram);
}

void App::unregisterShaderProgram(ShaderProgram* shaderProgram) {
	impl->shaderPrograms.erase(shaderProgram);
}

void App::updateProjectionMatrix() const {
	for (const auto shaderProgram : impl->shaderPrograms) {
		const auto context = shaderProgram->use();
		glUniformMatrix4fv(shaderProgram->getUniformLocation("projection"), 1, GL_FALSE,
		                   opengl::projection.data);
	}
}

#if !defined(__APPLE__) || !TARGET_OS_IPHONE // iOS
namespace internal {

void mainLoop(AppParameters params) {
	App app(params);
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
		params.screenSize = { static_cast<double>(getDesktopWidth()),
			                  static_cast<double>(getDesktopHeight()) };
		fullscreen = true;
	}
	if (fullscreen) {
		const Vec2 desktopSize{ static_cast<double>(getDesktopWidth()),
			                    static_cast<double>(getDesktopHeight()) };
		if (desktopSize.x > 0 &&
		    desktopSize.y > 0) { // desktop size isn't available on some platforms (e.g. Android)
			setScaleFactor(std::min(desktopSize.x / params.screenSize->x,
			                        desktopSize.y / params.screenSize->y));
		}
	} else {
		// Make window as big as possible
		const double scaleFactor = std::min((getDesktopWidth() - 99) / params.screenSize->x,
		                                    (getDesktopHeight() - 99) / params.screenSize->y);
		if (scaleFactor > 1) {
			setScaleFactor(std::floor(scaleFactor));
		} else {
			setScaleFactor(scaleFactor);
		}
	}
	showWindow(params.displayName,
	           fullscreen ? getDesktopWidth()
	                      : static_cast<int>(std::lround(params.screenSize->x * getScaleFactor())),
	           fullscreen ? getDesktopHeight()
	                      : static_cast<int>(std::lround(params.screenSize->y * getScaleFactor())),
	           fullscreen, params.minAspectRatio ? *params.minAspectRatio : minAspectRatio,
	           params.maxAspectRatio ? *params.maxAspectRatio : maxAspectRatio);
	setWork(params.start());
	app.mainLoop();
}

} // namespace internal
#endif

} // namespace jngl
