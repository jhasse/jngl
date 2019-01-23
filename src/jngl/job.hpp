// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// @file
#pragma once

#include <memory>

#include "dll.hpp"

namespace jngl {

class JNGLDLL_API Job {
public:
	/// Advance the game logic
	///
	/// This function gets called 60 times per second or whatever has been set by
	/// setStepsPerSecond().
	virtual void step() = 0;

	/// Draw the game state
	///
	/// As it isn't garuanteed that this function is called as often as Work::step, you shouldn't
	/// change any game state in it.
	virtual void draw() const = 0;

	virtual ~Job();
};

/// Add a new Job which will be always be stepped and drawn by mainLoop()
void JNGLDLL_API addJob(std::shared_ptr<Job> job);

} // namespace jngl
