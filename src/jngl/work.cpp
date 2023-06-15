// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "work.hpp"

#include "other.hpp"

namespace jngl {

void Work::onBackEvent() {
	onQuitEvent();
}

void Work::onQuitEvent() {
	jngl::quit();
}

void Work::onControllerBack() {
	onBackEvent();
}

void Work::onPauseEvent() {
}

void Work::onLoad() {
}

} // namespace jngl
