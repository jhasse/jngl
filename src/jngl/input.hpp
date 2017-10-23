// Copyright 2012-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <string>

#include "dll.hpp"

#ifndef _MSC_VER
#pragma GCC visibility push(default)
#endif

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
	void JNGLDLL_API setKeyboardVisible(bool);

	enum KeyboardType {
		Default,
		Numpad
	};

	/// Type of the onscreen keyboard
	void JNGLDLL_API setKeyboardType(KeyboardType);

	KeyboardType JNGLDLL_API getKeyboardType();

	void JNGLDLL_API setKeyPressed(const std::string& key, bool);

	void JNGLDLL_API setKeyPressed(key::KeyType key, bool);

	bool JNGLDLL_API keyDown(key::KeyType key);

	bool JNGLDLL_API keyDown(char key);

	bool JNGLDLL_API keyDown(const std::string& key);

	bool JNGLDLL_API keyPressed(key::KeyType key);

	bool JNGLDLL_API keyPressed(char key);

	bool JNGLDLL_API keyPressed(const std::string& key);

	void JNGLDLL_API setRelativeMouseMode(bool relative);

	bool JNGLDLL_API getRelativeMouseMode();

	void JNGLDLL_API setMouseVisible(bool visible);

	bool JNGLDLL_API isMouseVisible();

	bool JNGLDLL_API isMultitouch();

	int JNGLDLL_API getMouseX();

	int JNGLDLL_API getMouseY();

	namespace mouse {
		enum Button {
			Left, Middle, Right
		};
	}

	double JNGLDLL_API getMouseWheel();

	bool JNGLDLL_API mouseDown(mouse::Button button = mouse::Left);

	void JNGLDLL_API setMouseDown(mouse::Button button, bool);

	bool JNGLDLL_API mousePressed(mouse::Button button = mouse::Left);

	void JNGLDLL_API setMousePressed(jngl::mouse::Button, bool);

	void JNGLDLL_API setMouse(int xposition, int yposition);

	namespace controller {
		enum Button {
			LeftStickX,
			LeftStickY,
			RightStickX,
			RightStickY,
			A,
			B,
			X,
			Y,
			LeftButton,
			RightButton,
			LeftTrigger,
			RightTrigger,
			Start,
			Back,
			DpadUp,
			DpadDown,
			DpadLeft,
			DpadRight,
			LeftStick,
			RightStick,
			Last
		};
	}

	/// Check if the Controller is connected. Don't call this function every frame for performance reasons.
	bool JNGLDLL_API isControllerConnected(int number);

	/// Returns a value between 0.0f (not pressed) and 1.0f (pressed).
	float JNGLDLL_API getControllerState(int number, controller::Button);

	/// Returns true when the button is down.
	bool JNGLDLL_API getControllerDown(int number, controller::Button);

	/// Returns true (one time) when the button is pressed.
	bool JNGLDLL_API getControllerPressed(int number, controller::Button);

	/// Note that the right motor is the high-frequency motor, the left motor is the low-frequency motor.
	void JNGLDLL_API setControllerVibration(int number, unsigned short leftMotor, unsigned short rightMotor);
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
