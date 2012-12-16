/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "windowimpl.hpp"

#include "../window.hpp"
#include "debug.hpp"

namespace jngl {
	WindowImpl::WindowImpl(Window* w) : window(w) {
		debug("init with Window: "); debug(window); debug(" this: "); debug(this); debug("\n");
	}
	
	void WindowImpl::setMouse(int x, int y) {
		if (window->height_ == 640) {
			x *= 2;
			y *= 2;
		}
		if (flip) {
			mouseX = y;
			mouseY = window->height_ - x;
		} else {
			mouseX = window->width_ - y;
			mouseY = x;
		}
	}
	
	void WindowImpl::setMouseDown(bool d) {
		if (window->mouseDown_.at(jngl::mouse::Left)) {
			window->multitouch = d;
		}
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
	
	void WindowImpl::setFlip(bool f) {
		flip = f;
	}
}