// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include <boost/qvm/vec.hpp>
#include <string>
#include <vector>
#include <memory>

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

	boost::qvm::vec<double, 2> JNGLDLL_API getMousePos();

	[[deprecated("Use jngl::getMousePos() instead")]]
	int JNGLDLL_API getMouseX();

	[[deprecated("Use jngl::getMousePos() instead")]]
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

	class Controller;

	/// Returns all Controllers that are connected. Don't call this function every frame for performance reasons
	std::vector<std::shared_ptr<Controller>> JNGLDLL_API getConnectedControllers();
}
#ifndef _MSC_VER
#pragma GCC visibility pop
#endif
