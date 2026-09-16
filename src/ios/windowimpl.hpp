// Copyright 2012-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <cstddef>
#include <map>
#include <string>

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

	// Keyboard events arrive from UIKit asynchronously between frames, but
	// Window::updateKeyStates() (which runs at the start of every step) clears the text input and
	// the "pressed" flags. If we wrote to the Window directly, the next step would wipe the input
	// before the game's step() could read it. So we buffer the events here and replay them in
	// updateInput(), which runs right after updateKeyStates() and before the game steps.
	void enqueueTextInput(const std::string&); ///< a typed UTF-8 character (see jngl::getTextInput)
	void enqueueReturn(); ///< the Return/Enter key
	void enqueueBackspace(); ///< the Backspace/delete key

	int relativeX = 0;
	int relativeY = 0;
	void updateInput();

	std::map<int64_t, Vec2> touches;

	bool shouldCallControllerChangedCallback = false;
	std::map<void*, std::shared_ptr<AppleController>> appleControllers;

	void sleepIfNeeded();

private:
	Window* const window;
	int mouseX = 0;
	int mouseY = 0;

	std::string pendingTextInput;
	uint8_t pendingReturn = 0;
	uint8_t pendingBackspace = 0;
};

} // namespace jngl
