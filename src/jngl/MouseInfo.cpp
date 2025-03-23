// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "MouseInfo.hpp"

#include "input.hpp"

namespace jngl {

void MouseInfo::setMousePos(Vec2 mousePos) {
	enabled = true;
	this->mousePos = mousePos;
}

Vec2 MouseInfo::Down::newPos() const {
	return parent->mousePos - startReference;
}

bool MouseInfo::Down::released() { // NOLINT
	return !mouseDown();
}

Vec2 MouseInfo::Down::startPos() const {
	return startReference;
}

MouseInfo::Down::Down(MouseInfo& parent, Vec2 objectPos)
: parent(&parent), startReference(parent.mousePos - objectPos) {
	parent.down = true;
}

MouseInfo::Down::Down(Down&& other) noexcept
: parent(other.parent), startReference(other.startReference) {
	other.parent = nullptr;
}

auto MouseInfo::Down::operator=(Down&& other) noexcept -> Down& {
	parent = other.parent;
	startReference = other.startReference;
	other.parent = nullptr;
	return *this;
}

MouseInfo::Down::~Down() {
	if (parent) {
		parent->down = false;
	}
}

MouseInfo::Over::Over(MouseInfo& parent) : parent(parent) {
}

auto MouseInfo::Over::pressed(Vec2 objectPos) -> std::optional<Down> {
	parent.enabled = false;
	if (mousePressed()) {
		return Down{ parent, objectPos };
	}
	return std::nullopt;
}

Vec2 MouseInfo::Over::pos() const {
	return parent.mousePos;
}

auto MouseInfo::pos() -> std::optional<Over> {
	if (enabled && !down) {
		return Over{ *this };
	}
	return std::nullopt;
}

} // namespace jngl
