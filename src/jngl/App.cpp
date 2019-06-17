// Copyright 2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "App.hpp"

#include "../windowptr.hpp"

#include <stdexcept>

namespace jngl {

App* App::self = nullptr;

struct App::Impl {
	std::string displayName;
};

App::App(std::string displayName) : impl(new Impl{ std::move(displayName) }) {
	if (self) {
		throw std::runtime_error("You may only create one instance of jngl::App.");
	}
	self = this;
}

App::~App() {
	self = nullptr;
}

const App& App::instance() {
	if (!self) {
		throw std::runtime_error(
		    "jngl::App hasn't been created yet. Create an instance in your main function.");
	}
	return *self;
}

std::string App::getDisplayName() const {
	return impl->displayName;
}

void App::mainLoop() {
	return pWindow->mainLoop();
}

} // namespace jngl
