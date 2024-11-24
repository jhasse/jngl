// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "work.hpp"

#include "other.hpp"

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

} // namespace jngl
