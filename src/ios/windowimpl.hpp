// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <cstddef>
#include <map>

namespace jngl {
	class AppleController;
	class Vec2;
	class Window;

	class WindowImpl {
	public:
		WindowImpl(Window*);
		WindowImpl(const WindowImpl&) = delete;
		WindowImpl& operator=(const WindowImpl&) = delete;

		enum class Touch {
			BEGAN,
			ENDED,
			MOVED,
		};

		void handleTouch(const std::pair<intptr_t, Vec2>*, size_t count, Touch);
		//               ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TODO: use std::span

		void setMouseDown(bool);
		void setRelativeMouseMode(bool);
		int relativeX = 0;
		int relativeY = 0;
		void updateInput();

		std::map<int64_t, Vec2> touches;

		bool shouldCallControllerChangedCallback = false;
		std::map<void*, std::shared_ptr<AppleController>> appleControllers;

	private:
		Window* const window;
		int mouseX = 0;
		int mouseY = 0;
	};
}
