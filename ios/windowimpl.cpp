/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "windowimpl.hpp"
#include "jngl.hpp"
#include "window.hpp"
#include "debug.hpp"

namespace jngl {
	WindowImpl::WindowImpl(Window* w) : window(w) {
		Debug("init with Window: "); Debug(window); Debug(" this: "); Debug(this); Debug("\n");
	}
	
	void WindowImpl::setMouse(int x, int y) {
		Debug("Window: "); Debug(window); Debug(" this: "); Debug(this); Debug("\n");
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