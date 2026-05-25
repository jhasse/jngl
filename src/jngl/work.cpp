// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "work.hpp"

#include "../log.hpp"
#include "other.hpp"
#include "window.hpp"

namespace jngl {

void Work::onBackEvent() {
	jngl::quit();
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

void Work::onToggleFullscreen() {
	try {
		setFullscreen(!getFullscreen());
	} catch (const std::exception& e) {
		internal::error("Couldn't toggle fullscreen mode: {}", e.what());
	}
}

} // namespace jngl
