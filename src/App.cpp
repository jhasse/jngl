// Copyright 2019-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "App.hpp"

#include "windowptr.hpp"
#include "jngl/AppParameters.hpp"
#include "jngl/ShaderProgram.hpp"
#include "jngl/debug.hpp"

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

App::App(AppParameters params) : impl(new Impl{ params.displayName, params.pixelArt, params.steamAppId }) {
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

} // namespace jngl
