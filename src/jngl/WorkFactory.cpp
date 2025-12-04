// Copyright 2024-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "WorkFactory.hpp"
#include <cassert>

namespace jngl {

WorkFactory::WorkFactory(std::function<std::shared_ptr<Work>()> factory)
: factory(std::move(factory)) {
}

void WorkFactory::step() {
	assert(factory); // we should only be stepped once
	factory = nullptr;
	work->step();
}

void WorkFactory::draw() const {
	work->draw();
}

void WorkFactory::onLoad() {
	work = factory();
	jngl::setScene(work);
}

} // namespace jngl
