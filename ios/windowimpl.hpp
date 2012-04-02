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

#pragma once

#include <boost/noncopyable.hpp>

namespace jngl {
	class Window;
	
	class WindowImpl : boost::noncopyable {
	public:
		WindowImpl(Window*);
		void setMouse(int, int);
		void setMouseDown(bool);
		void setRelativeMouseMode(bool);
		int relativeX = 0;
		int relativeY = 0;
		void updateInput();
	private:
		Window* const window;
		int mouseX = 0;
		int mouseY = 0;
	};
}