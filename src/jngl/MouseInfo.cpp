// Copyright 2025-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "MouseInfo.hpp"

#include "Mat3.hpp"
#include "input.hpp"

namespace jngl {

struct MouseInfo::Impl {
	bool enabled = true;
	bool down = false;
	Vec2 mousePos;
};

void MouseInfo::transform(const Mat3& transformationMatrix) {
	for (auto& impl : impls) {
		auto inverse = boost::qvm::inverse(transformationMatrix);
		boost::qvm::vec<float, 3> homogeneous{ static_cast<float>(impl.mousePos.x),
			                                   static_cast<float>(impl.mousePos.y), 1.0f };
		auto transformed = inverse * homogeneous;
		impl.mousePos = Vec2(boost::qvm::A<0>(transformed), boost::qvm::A<1>(transformed));
	}
}

void MouseInfo::setMousePos(Vec2 mousePos) {
	impls.resize(1);
	impls[0].enabled = true;
	impls[0].mousePos = mousePos;
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

MouseInfo::Down::Down(MouseInfo::Impl& parent, Vec2 objectPos)
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

MouseInfo::Over::Over(MouseInfo::Impl& parent) : parent(parent) {
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

std::span<MouseInfo::Over> MouseInfo::cursors() {
	activeCursors.clear();
	for (auto& impl : impls) {
		if (impl.enabled && !impl.down) {
			activeCursors.emplace_back(impl);
		}
	}
	return activeCursors;
}

MouseInfo::MouseInfo() = default;
MouseInfo::~MouseInfo() = default;

} // namespace jngl
