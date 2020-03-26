// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "windowimpl.hpp"

#include "../window.hpp"
#include "../jngl/debug.hpp"

namespace jngl {
	WindowImpl::WindowImpl(Window* w) : window(w) {
		debug("init with Window: "); debug(window); debug(" this: "); debug(this); debug("\n");
	}

void WindowImpl::handleTouch(const std::pair<intptr_t, Vec2>* const positions, const size_t count, const Touch type) {
	for (size_t i = 0; i < count; ++i) {
		switch(type) {
			case Touch::BEGAN:
				touches.emplace(positions[i]);
				break;
			case Touch::MOVED:
				touches.find(positions[i].first)->second = positions[i].second;
				break;
			case Touch::ENDED:
				touches.erase(touches.find(positions[i].first));
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
		window->mousePressed_.at(jngl::mouse::Left) = d;
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
}
