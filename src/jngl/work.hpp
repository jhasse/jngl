// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "job.hpp"

#include <memory>

namespace jngl {

/// Active state of the game, e.g. a menu or the game itself
class Work : public Job {
public:
	/// Gets called when the "back" button is pressed (important on Android)
	///
	/// Calls onQuitEvent by default.
	virtual void onBackEvent();

	/// Gets called when the user closes the main window or quit() has been called
	///
	/// To continue with the main loop, call cancelQuit().
	virtual void onQuitEvent();

	/// Gets called when the "Back" button is pressed on any connected controller (Android only)
	///
	/// As the TV remote is registered as a controller this is important to handle Back on the TV
	/// remote. Most likely you won't need to overwrite this though, as it calls onBackEvent() by
	/// default and having Android's "Back" button to the same as "Back" on the TV remote is a good
	/// idea.
	virtual void onControllerBack();

	/// Gets called when the main window loses focus or the app is put in the background
	///
	/// On iOS and Android this will happen when the user switches to another app. As the main loop
	/// continues to run you might want to pause the game if necessary.
	virtual void onPauseEvent();

	/// Gets called when the Work is activated by App::mainLoop()
	virtual void onLoad();
};

/// Returns the current active Work or nullptr if none has been set
std::shared_ptr<Work> getWork();

/// Sets the passed Work to be active in App::mainLoop()
void setWork(std::shared_ptr<Work> work);

/// The same as setWork(std::shared_ptr<Work>) but creates the Work for you
template <class T, class... Args>
void setWork(Args&&... args) {
	setWork(std::make_shared<T>(std::forward<Args>(args)...));
}

/// \deprecated Use setWork(std::shared_ptr<Work>) instead
[[deprecated("Use setWork(std::shared_ptr<Work>) instead")]]
void setWork(Work*);

/// Resets the automatic frame limiter of App::mainLoop().
///
/// This is useful after you have done a huge amount of work in Work::draw, e.g. after loading
/// screens.
void resetFrameLimiter();

} // namespace jngl
