// Copyright 2017 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "SdlController.hpp"


namespace jngl {

SdlController::SdlController(SDL_Joystick* const handle) : handle(handle) {
}

SdlController::~SdlController() {
	SDL_JoystickClose(handle);
}

float SdlController::state(controller::Button button) const {
	const bool xboxWired = SDL_JoystickNumButtons(handle) == 11;
	int axisIndex;
	switch (button) {
		case controller::LeftStickX: axisIndex = 0; break;
		case controller::LeftStickY: axisIndex = 1; break;
		case controller::RightStickX: axisIndex = xboxWired ? 2 : 3; break;
		case controller::RightStickY: axisIndex = xboxWired ? 3 : 4; break;
		case controller::LeftTrigger: axisIndex = xboxWired ? 5 : 2; break;
		case controller::RightTrigger: axisIndex = xboxWired ? 4 : 5; break;
		default: return down(button);
	}
	float state = SDL_JoystickGetAxis(handle, axisIndex);
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

bool SdlController::down(const controller::Button button) const {
	//           DS4: 16 buttons
	// Xbox Wireless: 15 buttons
	//    Xbox Wired: 11 buttons
	const bool xbox = SDL_JoystickNumButtons(handle) != 16;
	if (xbox && (button == controller::LeftTrigger || button == controller::RightTrigger)) {
		return state(button) > 0;
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
		case controller::DpadUp: return SDL_JoystickGetHat(handle, 0) & SDL_HAT_UP;
		case controller::DpadDown: return SDL_JoystickGetHat(handle, 0) & SDL_HAT_DOWN;
		case controller::DpadLeft: return SDL_JoystickGetHat(handle, 0) & SDL_HAT_LEFT;
		case controller::DpadRight: return SDL_JoystickGetHat(handle, 0) & SDL_HAT_RIGHT;
		case controller::LeftStick: buttonIndex = xbox ? 9 : 11; break;
		case controller::RightStick: buttonIndex = xbox ? 10 : 12; break;
		default: return false;
	}
	return SDL_JoystickGetButton(handle, buttonIndex);
}

bool SdlController::is(SDL_Joystick* const handle) const {
	return this->handle == handle;
}

} // namespace jngl
