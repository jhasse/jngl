// Copyright 2019-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "App.hpp"

#include "../windowptr.hpp"
#include "debug.hpp"
#include "ShaderProgram.hpp"

#include <set>
#include <stdexcept>

namespace jngl {

App* App::self = nullptr;

struct App::Impl {
	std::string displayName;
	bool pixelArt = false;
	std::set<ShaderProgram*> shaderPrograms;
};

App::App(std::string displayName) : impl(new Impl{ std::move(displayName) }) {
	if (self) {
		throw std::runtime_error("You may only create one instance of jngl::App.");
	}
	self = this;
}

App::App() : impl(new Impl) {
	if (self) {
		throw std::runtime_error("You may only create one instance of jngl::App.");
	}
}

App::~App() {
	self = nullptr;
}

App& App::instance() {
	if (!self) {
		self = new App; // TODO: Don't leak, but turn this into a "real" singleton
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
