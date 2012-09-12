/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
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
		void setFlip(bool);
		int relativeX = 0;
		int relativeY = 0;
		void updateInput();
	private:
		Window* const window;
		int mouseX = 0;
		int mouseY = 0;
		bool flip = false;
	};
}