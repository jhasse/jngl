// Copyright 2012-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include "dll.hpp"
#include "job.hpp"

#include <memory>

namespace jngl {

/// Active state of the game, e.g. a menu or the game itself
class JNGLDLL_API Work : public Job {
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

std::shared_ptr<Work> JNGLDLL_API getWork();

/// Sets the passed Work to be active in App::mainLoop()
void JNGLDLL_API setWork(std::shared_ptr<Work> work);

/// \deprecated Use setWork(std::shared_ptr<Work>) instead
[[deprecated("Use setWork(std::shared_ptr<Work>) instead")]]
void JNGLDLL_API setWork(Work*);

void JNGLDLL_API resetFrameLimiter();

} // namespace jngl
