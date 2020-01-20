// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "dll.hpp"
#include "job.hpp"

#include <memory>

namespace jngl {

/// Active state of the game, e.g. a menu or the game itself
class Work : public Job {
public:
	/// Gets called when the user closes the main window or quit() has been called
	///
	/// On Android this will happen when the back button is pressend, on iOS when the app is closed
	/// or put in the background.
	///
	/// To continue with the main loop, call cancelQuit().
	virtual void onQuitEvent();

	/// Gets called when the Work is activated by App::mainLoop()
	virtual void onLoad();
};

/// Returns the current active Work or nullptr if none has been set
std::shared_ptr<Work> getWork();

/// Sets the passed Work to be active in App::mainLoop()
void setWork(std::shared_ptr<Work> work);

/// \deprecated Use setWork(std::shared_ptr<Work>) instead
[[deprecated("Use setWork(std::shared_ptr<Work>) instead")]]
void setWork(Work*);

/// Resets the automatic frame limiter of App::mainLoop().
///
/// This is useful after you have done a huge amount of work in Work::draw, e.g. after loading
/// screens.
void resetFrameLimiter();

} // namespace jngl
