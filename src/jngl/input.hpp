// Copyright 2012-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Input related functions
/// @file
#pragma once

#include "Finally.hpp"
#include "Vec2.hpp"

#include <functional>
#include <memory>
#include <vector>
#if defined(__has_include) && __has_include(<optional>)
#include <optional>
using std::optional;
#else
#include <experimental/optional>
using std::experimental::optional;
#endif

namespace jngl {

namespace key {
/// Keyboard keys
enum KeyType : uint8_t {
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
	CtrlL,
	CtrlR,
	ControlL = CtrlL,
	ControlR = CtrlR,

	/// Shortcut for either CtrlL or CtrlR
	Ctrl,

	CapsLock,

	/// Left Alt key
	Alt,

	/// Right Alt key (Alt Graph key)
	AltGr,

	AltL = Alt,
	AltR = AltGr,

	/// Left Windows key
	SuperL,

	/// Right Windows key
	SuperR,

	Space,
	ShiftL,
	ShiftR,

	/// Shortcut for either ShiftL or ShiftR
	Shift,

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

/// Returns a string representation of a key useful for displaying it to the user
std::string keyToString(key::KeyType);

/// Display onscreen keyboard for touch devices
void setKeyboardVisible(bool);

/// Whether a normal or a numbers-only keyboard should appear
enum KeyboardType : uint8_t { Default, Numpad };

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

/// Whether \p key is down, case-insensitive for A-Z
///
/// \note jngl::keyDown('a') and jngl::keyDown('A') will both return true regardless of the state of
/// the <kbd>Shift</kbd> key. This is different from jngl::keyDown(const std::string&) which is
/// case-sensitive.
///
/// Example:
/// \code
/// if (jngl::keyDown('d')) {
///     playerPos.x += 1;
/// }
/// if (jngl::keyDown('a')) {
///     playerPos.x -= 1;
/// }
/// if (jngl::keyDown('s')) {
///     playerPos.y += 1;
/// }
/// if (jngl::keyDown('w')) {
///     playerPos.y -= 1;
/// }
/// \endcode
bool keyDown(char key);

/// Whether \p key is down, where \p key should be exactly one UTF-8 character
///
/// \note `jngl::keyDown("A")` will only return true if the user presses <kbd>Shift</kbd> and
/// <kbd>A</kbd> at the same time. This is different from jngl::keyDown(char) which is
/// case-insensitive for A-Z.
///
/// Example:
/// \code
/// if (jngl::keyDown("a")) {
///     player1Pos.x += 1;
/// }
/// if (jngl::keyDown("s")) {
///     player1Pos.x -= 1;
/// }
/// if (jngl::keyDown("ö")) {
///     player2Pos.x += 1;
/// }
/// if (jngl::keyDown("ä")) {
///     player2Pos.x -= 1;
/// }
/// \endcode
bool keyDown(const std::string& key);

/// Whether \p key has been pressed since the next to last call to updateInput()
bool keyPressed(key::KeyType key);

/// Whether \p key has been pressed since the next to last call to updateInput()
///
/// \note jngl::keyPressed('a') and jngl::keyPressed('A') will both return true regardless of the
/// state of the <kbd>Shift</kbd> key. This is different from jngl::keyPressed(const std::string&)
/// which is case-sensitive.
bool keyPressed(char key);

/// Whether \p key has been pressed since the next to last call to updateInput(), where \p key
/// should be exactly one UTF-8 character
///
/// \note `jngl::keyPressed("A")` will only return true if the user presses <kbd>Shift</kbd> and A
/// at the same time. This is different from jngl::keyPressed(char) which is case-insensitive for
/// A-Z.
bool keyPressed(const std::string& key);

/// Enables relative mouse mode which will hide the cursor and allow for unlimited movement
///
/// When enabled getMousePos() will return the relative movement in the current step.
void setRelativeMouseMode(bool relative);

/// Whether relative mouse mode is currently active
bool getRelativeMouseMode();

/// By default the mouse cursor of the OS is visible and can be hidden by passing false
void setMouseVisible(bool visible);

/// Hides the mouse cursor; destroying the returned Finally object will show it again
[[nodiscard]] Finally hideMouse();

/// Returns whether the mouse cursor of the OS is currently visible
///
/// Even when this method returns true, it could still be visible outside of the window or hidden by
/// the OS for other means (e.g. while the user is typing).
bool isMouseVisible();

/// Returns true when there's more than one finger touching the screen
///
/// Equivalent to `jngl::getTouchPositions().size() > 1`.
bool isMultitouch();

/// Returns all positions where a finger touches the screen. Includes the mouse position if the
/// primary mouse button is down.
std::vector<jngl::Vec2> getTouchPositions();

/// Mouse position in screen coordinates
///
/// If no mouse is connected will return the last touch position. See jngl::getCursorPos() if you
/// really want the position of the mouse only.
Vec2 getMousePos();

/// Returns the position of the mouse pointer if a mouse is connected/available
///
/// On Android, iOS and for the Nintendo Switch this will always return std::nullopt. While Android
/// supports connecting mice this isn't supported by JNGL yet.
optional<Vec2> getCursorPos();

/// Retrieve mouse position in pixels
/// \deprecated Use jngl::getMousePos() instead
[[deprecated("Use jngl::getMousePos() instead")]] int getMouseX();

/// Retrieve mouse position in pixels
/// \deprecated Use jngl::getMousePos() instead
[[deprecated("Use jngl::getMousePos() instead")]] int getMouseY();

namespace mouse {
/// Mouse buttons
enum Button : uint8_t { Left, Middle, Right };
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
enum Button : uint8_t {
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
///
/// \deprecated Use jngl::Job::onControllersChanged() instead
[[deprecated("Use jngl::Job::onControllersChanged() instead")]]
void onControllerChanged(std::function<void()> callback);

/// Returns a string of characters that have been pressed since the last call to updateInput()
std::string getTextInput();

} // namespace jngl
