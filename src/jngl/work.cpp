// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "work.hpp"

#include "other.hpp"

namespace jngl {

void Work::onQuitEvent() {
	jngl::quit();
}

void Work::onPauseEvent() {
}

void Work::onLoad() {
}

} // namespace jngl
