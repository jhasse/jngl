// Copyright 2019-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::App class
/// @file
#pragma once

#include "jngl/Finally.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace jngl {
struct AppParameters;
class ShaderProgram;

/// Singleton, that never gets destroyed
class App {
public:
	~App();
	App(const App&) = delete;
	App& operator=(const App&) = delete;
	App(App&&) = delete;
	App& operator=(App&&) = delete;

	/// Access the instance
	static App& instance();

	/// The display name of the app is used by jngl::writeConfig() for example.
	[[nodiscard]] std::string getDisplayName() const;

	/// \deprecated Set jngl::AppParameters.displayName instead
	void setDisplayName(const std::string&);

	/// Starts the main loop, which calls jngl::Work::step and jngl::Work::draw
	void mainLoop();

	/// Activates pixel-perfect magnifying of textures (nearest-neighbor interpolation)
	///
	/// It's important to call this before textures are loaded. Defaults to false.
	void setPixelArt(bool);

	/// If pixel-perfect magnifying is activated (see setPixelArt)
	static bool isPixelArt();

	/// Internal function used by JNGL when the Window is resized
	void updateProjectionMatrix() const;

	/// Initializes Impl, the returned Finally will destroy it again
	[[nodiscard]] Finally init(AppParameters);

	// TODO for C++23: Change to std::move_only_function<void() noexcept>
	void atExit(std::function<void()>);
	void callAtExitFunctions();

private:
	App();

	void registerShaderProgram(ShaderProgram*);
	void unregisterShaderProgram(ShaderProgram*);

	friend ShaderProgram;

	struct Impl;
	std::unique_ptr<Impl> impl;

	static App* self;

	/// Not part of Impl so that jngl::atExit works even before jnglInit() has been called.
	/// The lifecycle order is the following:
	///
	/// * App::App()
	/// * jnglInit()
	/// * App::init()
	/// * Window::Window()
	/// * appParameters.start()
	std::vector<std::function<void()>> callAtExit;
};

} // namespace jngl
