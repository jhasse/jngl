// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "all.hpp"

namespace jngl {

void Work::onQuitEvent() {
	jngl::quit();
}

void Work::onLoad() {
}

Work::~Work() = default;

} // namespace jngl
