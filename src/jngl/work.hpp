// Copyright 2012-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "job.hpp"

#include <memory>

namespace jngl {

/// Active state of the game, e.g. a menu or the game itself
class Work : public Job {
public:
	/// Gets called on Android's "back" gesture or when the <kbd>Esc</kbd> key is pressed
	///
	/// Calls quit() by default, so exits the game. It's a good idea that you would only use the
	/// default behavior in the main menu though. While in-game you normally would switch to a pause
	/// menu. In the pause menu onBackEvent() would go to the main menu's Work.
	///
	/// Example:
	/// \code
	/// class MainMenu : public jngl::Work {
	///     // default implementation of onBackEvent which quits the game
	/// };
	///
	/// class PauseMenu : public jngl::Work {
	///     void onBackEvent() override {
	///         jngl::setWork<MainMenu>();
	///     }
	/// };
	///
	/// class Game : public jngl::Work {
	///     void onBackEvent() override {
	///         jngl::setWork<PauseMenu>();
	///     }
	/// };
	/// \endcode
	virtual void onBackEvent();

	/// Gets called when the user closes the main window or quit() has been called
	///
	/// To continue with the main loop, call cancelQuit(). For example if you want to switch to a
	/// pause menu instead of exiting the game:
	///
	/// \code
	/// class Game : public jngl::Work {
	///     void onQuitEvent() {
	///         jngl::cancelQuit();
	///         jngl::setWork<PauseMenu>();
	///     }
	/// };
	/// \endcode
	virtual void onQuitEvent();

	/// Gets called when the "Back" button is pressed on any connected controller (Android only)
	///
	/// As the TV remote is registered as a controller this is important to handle Back on the TV
	/// remote. Most likely you won't need to overwrite this though, as it calls onBackEvent() by
	/// default and having Android's "Back" button behave the same as "Back" on the TV remote is a
	/// good idea.
	virtual void onControllerBack();

	/// Gets called when the main window loses focus or the app is put in the background
	///
	/// On iOS and Android this will happen when the user switches to another app. As the main loop
	/// continues to run you might want to pause the game if necessary.
	virtual void onPauseEvent();

	/// Gets called when the Work is activated by the main loop
	virtual void onLoad();

	/// Gets called when the Work is deactivated by the main loop
	virtual void onUnload();
};

/// Returns the current active Work or nullptr if none has been set
std::shared_ptr<Work> getWork();

/// Sets the passed Work to be active in the main loop
/// \deprecated Use setScene(std::shared_ptr<Scene>) instead
[[deprecated("Use setScene(std::shared_ptr<Scene>) instead")]]
void setWork(std::shared_ptr<Work> work);

/// The same as setWork(std::shared_ptr<Work>) but creates the Work for you
template <class T, class... Args>
T& setWork(Args&&... args) {
	auto shared = std::make_shared<T>(std::forward<Args>(args)...);
	auto& rtn = *shared;
	setScene(std::move(shared));
	return rtn;
}

/// Resets the automatic frame limiter of App::mainLoop().
///
/// This is useful after you have done a huge amount of work in Work::draw, e.g. after loading
/// screens.
void resetFrameLimiter();

} // namespace jngl
