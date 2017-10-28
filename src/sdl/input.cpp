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
		const bool xboxWired = SDL_JoystickNumButtons(it->second) == 11;
		int axisIndex;
		switch (button) {
			case controller::LeftStickX: axisIndex = 0; break;
			case controller::LeftStickY: axisIndex = 1; break;
			case controller::RightStickX: axisIndex = xboxWired ? 2 : 3; break;
			case controller::RightStickY: axisIndex = xboxWired ? 3 : 4; break;
			case controller::LeftTrigger: axisIndex = xboxWired ? 5 : 2; break;
			case controller::RightTrigger: axisIndex = xboxWired ? 4 : 5; break;
			default: return getControllerDown(number, button);
		}
		float state = SDL_JoystickGetAxis(it->second, axisIndex);
		if (state < 0) {
			state /= 32768;
		} else {
			state /= 32767;
		}
		if (button == controller::LeftStickY or button == controller::RightStickY) {
			state *= -1;
		}
		return state;
	}

	bool getControllerDown(const int number, const controller::Button button) {
		const auto it = joysticks.find(number);
		if (it == joysticks.end()) {
			return false;
		}
		//           DS4: 16 buttons
		// Xbox Wireless: 15 buttons
		//    Xbox Wired: 11 buttons
		const bool xbox = SDL_JoystickNumButtons(it->second) != 16;
		if (xbox && (button == controller::LeftTrigger || button == controller::RightTrigger)) {
			return getControllerState(number, button) > 0;
		}
		int buttonIndex;
		switch (button) {
			case controller::A: buttonIndex = 0; break;
			case controller::B: buttonIndex = 1; break;
			case controller::X: buttonIndex = xbox ? 2 : 3; break;
			case controller::Y: buttonIndex = xbox ? 3 : 2; break;
			case controller::LeftButton: buttonIndex = 4; break;
			case controller::RightButton: buttonIndex = 5; break;
			case controller::LeftTrigger: buttonIndex = 6; break;
			case controller::RightTrigger: buttonIndex = 7; break;
			case controller::Start: buttonIndex = xbox ? 7 : 9; break;
			case controller::Back: buttonIndex = xbox ? 6 : 8; break;
			case controller::DpadUp: return SDL_JoystickGetHat(it->second, 0) & SDL_HAT_UP;
			case controller::DpadDown: return SDL_JoystickGetHat(it->second, 0) & SDL_HAT_DOWN;
			case controller::DpadLeft: return SDL_JoystickGetHat(it->second, 0) & SDL_HAT_LEFT;
			case controller::DpadRight: return SDL_JoystickGetHat(it->second, 0) & SDL_HAT_RIGHT;
			case controller::LeftStick: buttonIndex = xbox ? 9 : 11; break;
			case controller::RightStick: buttonIndex = xbox ? 10 : 12; break;
			default: return false;
		}
		return SDL_JoystickGetButton(it->second, buttonIndex);
	}

	void setControllerVibration(int, unsigned short, unsigned short) {
	}
}
