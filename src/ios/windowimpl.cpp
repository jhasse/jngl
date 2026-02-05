// Copyright 2012-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "windowimpl.hpp"

#include "../log.hpp"
#include "../window.hpp"

namespace jngl {
WindowImpl::WindowImpl(Window* w) : window(w) {
	internal::debug("init with Window: {:x} this: {:x}", reinterpret_cast<uintptr_t>(window),
	                reinterpret_cast<uintptr_t>(this));
}

void WindowImpl::handleTouch(const std::pair<intptr_t, Vec2>* const positions, const size_t count,
                             const Touch type) {
	for (size_t i = 0; i < count; ++i) {
		switch (type) {
		case Touch::BEGAN:
			touches.emplace(positions[i]);
			break;
		case Touch::MOVED:
			touches.find(positions[i].first)->second = positions[i].second;
			break;
		case Touch::ENDED:
			touches.erase(touches.find(positions[i].first));
			if (touches.size() == 1) {
				// For the case that the mouse position pointed to this pointer we need to update it
				// so that it still makes sense. This is very important when going from 2 pointer to
				// 1 since getTouchPositions() will switch to getMousePos() then.
				mouseX = touches.begin()->second.x;
				mouseY = touches.begin()->second.y;
			}
			break;
		}
	}
	if (touches.empty()) {
		setMouseDown(false);
	} else {
		setMouseDown(true);
		if (type != Touch::ENDED) {
			mouseX = positions[0].second.x;
			mouseY = positions[0].second.y;
		}
	}
	if (type != Touch::MOVED) {
		window->multitouch = touches.size() > 1;
	}
}

void WindowImpl::setMouseDown(bool d) {
	if (!d || !window->mouseDown_.at(jngl::mouse::Left)) {
		if ((window->mousePressed_.at(mouse::Left) = d)) {
			window->addUpdateInputCallback(
			    [this]() { window->needToBeSetFalse_.push(&window->mousePressed_[mouse::Left]); });
		}
	}
	window->mouseDown_.at(jngl::mouse::Left) = d;
}

void WindowImpl::setRelativeMouseMode(bool r) {
	if (r) {
		relativeX = window->mousex_;
		relativeY = window->mousey_;
		window->mousex_ = 0;
		window->mousey_ = 0;
	} else {
		relativeX = relativeY = 0;
	}
}

void WindowImpl::updateInput() {
	window->mousex_ = mouseX - relativeX;
	window->mousey_ = mouseY - relativeY;
	if (window->relativeMouseMode) {
		relativeX = mouseX;
		relativeY = mouseY;
	}
}

void WindowImpl::sleepIfNeeded() {
	window->frameLimiter.sleepIfNeeded();
}

} // namespace jngl
