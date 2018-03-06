// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

namespace jngl {
	class Window;

	class WindowImpl {
	public:
		WindowImpl(Window*);
		WindowImpl(const WindowImpl&) = delete;
		WindowImpl& operator=(const WindowImpl&) = delete;
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
