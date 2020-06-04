// Copyright 2012-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Input related functions
/// @file
#pragma once

#include "Vec2.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace jngl {

namespace key {
/// Keyboard keys
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

	/// Left Windows key
	SuperL,

	/// Right Windows key
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
	Any
};
} // namespace key

/// Display onscreen keyboard for touch devices
void setKeyboardVisible(bool);

/// Whether a normal or a numbers-only keyboard should appear
enum KeyboardType { Default, Numpad };

/// Type of the onscreen keyboard
void setKeyboardType(KeyboardType);

/// Currently active type of onscreen keyboard
KeyboardType getKeyboardType();

/// Overwrite what keyPressed() should return
void setKeyPressed(const std::string& key, bool);

/// Overwrite what keyPressed() should return
void setKeyPressed(key::KeyType key, bool);

/// Whether \p key is down
bool keyDown(key::KeyType key);

/// Whether \p key is down
bool keyDown(char key);

/// Whether \p key is down, where \p key should be exactly one UTF-8 character
bool keyDown(const std::string& key);

/// Whether \p key has been pressed since the next to last call to updateInput()
bool keyPressed(key::KeyType key);

/// Whether \p key has been pressed since the next to last call to updateInput()
bool keyPressed(char key);

/// Whether \p key has been pressed since the next to last call to updateInput(), where \p key
/// should be exactly one UTF-8 character
bool keyPressed(const std::string& key);

void setRelativeMouseMode(bool relative);

bool getRelativeMouseMode();

void setMouseVisible(bool visible);

bool isMouseVisible();

/// Returns true when there's more than one finger touching the screen
bool isMultitouch();

/// Returns all positions where a finger touches the screen. Includes the mouse position if the
/// primary mouse button is down.
std::vector<jngl::Vec2> getTouchPositions();

/// Mouse position in screen coordinates
Vec2 getMousePos();

/// Retrieve mouse position in pixels
/// \deprecated Use jngl::getMousePos() instead
[[deprecated("Use jngl::getMousePos() instead")]] int getMouseX();

/// Retrieve mouse position in pixels
/// \deprecated Use jngl::getMousePos() instead
[[deprecated("Use jngl::getMousePos() instead")]] int getMouseY();

namespace mouse {
/// Mouse buttons
enum Button { Left, Middle, Right };
} // namespace mouse

/// Returns mouse wheel movement between -100 and 100 (0 if the mousewheel doesn't move)
double getMouseWheel();

/// Returns whether \a button is currently held down
bool mouseDown(mouse::Button button = mouse::Left);

/// Overwrite what mouseDown() should return
void setMouseDown(mouse::Button button, bool);

/// Whether \a button has been pressed since the next to last call to updateInput()
bool mousePressed(mouse::Button button = mouse::Left);

/// Overwrite what mouseDown() should return
void setMousePressed(jngl::mouse::Button, bool);

/// Moves the mouse (does nothing on iOS and Android)
void setMouse(Vec2 position);

namespace controller {
/// Gamepad buttons
enum Button {
	/// x axis of left stick, -1 to 1
	LeftStickX,

	/// -LeftStickX
	LeftStickXInverse,

	/// y axis of left stick, -1 to 1
	LeftStickY,

	/// -LeftStickY
	LeftStickYInverse,

	/// x axis of right stick, -1 to 1
	RightStickX,

	/// -RightStickX
	RightStickXInverse,

	/// y axis of right stick, -1 to 1
	RightStickY,

	/// -RightStickY
	RightStickYInverse,

	A,
	B,
	X,
	Y,

	/// Left shoulder button
	LeftButton,

	/// Right shoulder button
	RightButton,

	LeftTrigger,
	RightTrigger,

	/// Start, or Options button
	Start,

	/// Back, Menu, or Share button
	Back,

	DpadUp,
	DpadDown,
	DpadLeft,
	DpadRight,
	LeftStick,
	RightStick,
	Last
};
} // namespace controller

class Controller;

/// Returns all controllers (gamepads) that are connected
///
/// Don't call this function every frame for performance reasons, use jngl::onControllerChanged
/// instead to get notified when controlles are being connected or removed.
std::vector<std::shared_ptr<Controller>> getConnectedControllers();

/// Specify a function which gets called, whenever a controller gets added or removed
///
/// Whenever the return value of jngl::getConnectedControllers would change, \a callback gets
/// called.
void onControllerChanged(std::function<void()> callback);

/// Returns a string of characters that have been pressed since the last call to updateInput()
std::string getTextInput();

} // namespace jngl
