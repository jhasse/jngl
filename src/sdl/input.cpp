// Copyright 2012-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/input.hpp"
#include "sdl.hpp"
#include "../jngl/debug.hpp"

#include <map>

namespace jngl {
	void setKeyboardVisible(bool) {
	}

	std::map<int, SDL_Joystick*> joysticks;

	bool isControllerConnected(const int number) {
		const auto it = joysticks.find(number);
		if (it == joysticks.end()) {
			if (number >= SDL_NumJoysticks()) {
				return false;
			}
			joysticks[number] = SDL_JoystickOpen(number);

			jngl::debug("Name: ");
			jngl::debugLn(SDL_JoystickNameForIndex(number));
			jngl::debug("Number of Axes: ");
			jngl::debugLn(SDL_JoystickNumAxes(joysticks[number]));
			jngl::debug("Number of Buttons: ");
			jngl::debugLn(SDL_JoystickNumButtons(joysticks[number]));
			jngl::debug("Number of Balls: ");
			jngl::debugLn(SDL_JoystickNumBalls(joysticks[number]));
			jngl::debug("Number of Hats: ");
			jngl::debugLn(SDL_JoystickNumHats(joysticks[number]));

			return true;
		}
		return SDL_JoystickGetAttached(it->second);
	}

	float getControllerState(const int number, controller::Button button) {
		const auto it = joysticks.find(number);
		if (it == joysticks.end()) {
			return 0.0f;
		}
		int axisIndex;
		switch (button) {
			case controller::LeftStickX: axisIndex = 0; break;
			case controller::LeftStickY: axisIndex = 1; break;
			case controller::RightStickX: axisIndex = 3; break;
			case controller::RightStickY: axisIndex = 4; break;
			case controller::LeftTrigger: axisIndex = 2; break;
			case controller::RightTrigger: axisIndex = 5; break;
			default: return getControllerPressed(number, button);
		}
		float state = SDL_JoystickGetAxis(it->second, axisIndex);
		if (state < 0) {
			state /= 32768;
		} else {
			state /= 32767;
		}
		return state / 2.0f + 0.5f;
	}

	bool getControllerPressed(const int number, const controller::Button button) {
		const auto it = joysticks.find(number);
		if (it == joysticks.end()) {
			return false;
		}
		int buttonIndex;
		switch (button) {
			case controller::A: buttonIndex = 0; break;
			case controller::B: buttonIndex = 1; break;
			case controller::X: buttonIndex = 3; break;
			case controller::Y: buttonIndex = 2; break;
			case controller::LeftButton: buttonIndex = 4; break;
			case controller::RightButton: buttonIndex = 5; break;
			case controller::LeftTrigger: buttonIndex = 6; break;
			case controller::RightTrigger: buttonIndex = 7; break;
			case controller::Start: buttonIndex = 9; break;
			case controller::Back: buttonIndex = 8; break;
			case controller::DpadUp: return SDL_JoystickGetHat(it->second, 0) & SDL_HAT_UP;
			case controller::DpadDown: return SDL_JoystickGetHat(it->second, 0) & SDL_HAT_DOWN;
			case controller::DpadLeft: return SDL_JoystickGetHat(it->second, 0) & SDL_HAT_LEFT;
			case controller::DpadRight: return SDL_JoystickGetHat(it->second, 0) & SDL_HAT_RIGHT;
			case controller::LeftStick: buttonIndex = 11; break;
			case controller::RightStick: buttonIndex = 12; break;
			default: return false;
		}
		return SDL_JoystickGetButton(it->second, buttonIndex);
	}

	void setControllerVibration(int, unsigned short, unsigned short) {
	}
}
