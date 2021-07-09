// Copyright 2019-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include <memory>
#include <string>

namespace jngl {
class ShaderProgram;

/// There can only be one instance of this class which will be created before the window is shown.
///
/// Example:
/// \code
/// #include <jngl/App.hpp>
/// #include <jngl/main.hpp>
///
/// JNGL_MAIN_BEGIN {
/// 	jngl::App::instance().setDisplayName("Awesome Game Name");
/// 	// ...
/// 	jngl::App::instance().mainLoop();
/// } JNGL_MAIN_END
/// \endcode
class App {
public:
	[[deprecated("Use jngl::App::instance().setDisplayName instead")]] explicit App(
	    std::string displayName);
	~App();
	App(const App&) = delete;
	App& operator=(const App&) = delete;
	App(App&&) = delete;
	App& operator=(App&&) = delete;

	/// Access the instance, creates it if it doesn't exist
	static App& instance();

	/// The display name of the app is used by jngl::writeConfig() for example.
	[[nodiscard]] std::string getDisplayName() const;

	void setDisplayName(const std::string&);

	/// Starts the main loop, which calls jngl::Work::step and jngl::Work::draw
	void mainLoop();

	/// Activates pixel-perfect magnifying of textures (nearest-neighbor interpolation)
	///
	/// It's important to call this before textures are loaded. Defaults to false.
	void setPixelArt(bool);

	/// If pixel-perfect magnifying is activated (see setPixelArt)
	static bool isPixelArt();

	void updateProjectionMatrix() const;

private:
	App();

	void registerShaderProgram(ShaderProgram*);
	void unregisterShaderProgram(ShaderProgram*);

	friend ShaderProgram;

	struct Impl;
	std::unique_ptr<Impl> impl;

	static App* self;
};

} // namespace jngl
