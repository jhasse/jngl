// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "effects.hpp"

#include "matrix.hpp"
#include "other.hpp"

namespace jngl {

Effect::~Effect() = default;

Zoom::Zoom(std::function<float(float)> function)
: function(std::move(function)) {
}

void Zoom::beginDraw() const {
	jngl::scale(function(time));
}

void Zoom::endDraw() const {
}

Zoom::Action Zoom::step() {
	time += 1.f / float(getStepsPerSecond());
	return Action::NONE;
}

Executor::Executor(std::function<Action(float)> function) : function(std::move(function)) {
}

void Executor::beginDraw() const {
}

void Executor::endDraw() const {
}

Executor::Action Executor::step() {
	time += 1.f / float(getStepsPerSecond());
	return function(time);
}

} // namespace jngl
