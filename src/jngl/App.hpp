// Copyright 2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <memory>
#include <string>

namespace jngl {

/// There can only be one instance of this class which should be created before the window is shown.
///
/// Example:
/// \code
/// #include <jngl/App.hpp>
/// #include <jngl/main.hpp>
///
/// JNGL_MAIN_BEGIN {
/// 	jngl::App app("Awesome Game Name");
/// 	// ...
/// 	app.mainLoop();
/// } JNGL_MAIN_END
/// \endcode
class App {
public:
	App(std::string displayName);
	~App();
	App(const App&) = delete;
	App& operator=(const App&) = delete;
	App(App&&) = delete;
	App& operator=(App&&) = delete;

	/// Access the instance, throws std::runtime_error if it doesn't exist.
	static const App& instance();

	/// Returns the name set by the constructor. This cannot be changed later and is used by
	/// jngl::getConfigPath() for example.
	std::string getDisplayName() const;

	/// Starts the main loop, which calls jngl::Work::step and jngl::Work::draw
	void mainLoop();

private:
	struct Impl;
	std::unique_ptr<Impl> impl;

	static App* self;
};

} // namespace jngl
