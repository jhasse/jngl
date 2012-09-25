/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include <string>

namespace jngl {
	namespace key {
		enum KeyType {
			Left,
			Up,
			Right,
			Down,
			PageUp,
			PageDown,
			Home,
			End,
			BackSpace,
			Tab,
			Clear,
			Return,
			Pause,
			Escape,
			Delete,
			ControlL,
			ControlR,
			CapsLock,
			AltL,
			AltR,
			SuperL,
			SuperR,
			Space,
			ShiftL,
			ShiftR,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			WizUp,
			WizDown,
			WizLeft,
			WizRight,
			WizA,
			WizB,
			WizX,
			WizY,
			WizL,
			WizR,
			WizMenu,
			WizSelect,
			WizVolUp,
			WizVolDown,
			Any
		};
	}

	/// Display onscreen keyboard for touch devices
	void setKeyboardVisible(bool);

	void setKeyPressed(const std::string& key, bool);

	void setKeyPressed(key::KeyType key, bool);

	bool keyDown(key::KeyType key);

	bool keyDown(char key);

	bool keyDown(const std::string& key);

	bool keyPressed(key::KeyType key);

	bool keyPressed(char key);

	bool keyPressed(const std::string& key);

	void setRelativeMouseMode(bool relative);

	bool getRelativeMouseMode();

	void setMouseVisible(bool visible);

	bool isMouseVisible();

	int getMouseX();

	int getMouseY();

	namespace mouse {
		enum Button {
			Left, Middle, Right
		};
	}

	double getMouseWheel();

	bool mouseDown(mouse::Button button = mouse::Left);

	void setMouseDown(mouse::Button button, bool);

	bool mousePressed(mouse::Button button = mouse::Left);

	void setMousePressed(jngl::mouse::Button, bool);

	void setMouse(int xposition, int yposition);
}