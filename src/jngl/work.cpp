// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "work.hpp"

#include "other.hpp"

namespace jngl {

void Work::onBackEvent() {
	if (canQuit()) {
		quit();
	} else {
		/// On platforms like the web quit() won't do anything. If the game uses onQuitEvent() to
		/// show a pause menu, call it manually here so that pressing Escape (which calls
		/// onBackEvent()) will open the pause menu instead of doing nothing.
		onQuitEvent();
	}
}

void Work::onQuitEvent() {
}

void Work::onControllerBack() {
	onBackEvent();
}

void Work::onPauseEvent() {
}

void Work::onLoad() {
}

void Work::onUnload() {
}

} // namespace jngl
