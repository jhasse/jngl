// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "XinputController.hpp"

#include "../window.hpp"

namespace jngl {

XinputController::XinputController(const int number) : i(number) {
}

float XinputController::state(const controller::Button b) const {
	const float max = 32767;

	using namespace controller;
	switch(b) {
		case LeftStickX: return float(states[i].Gamepad.sThumbLX)/max;
		case LeftStickY: return float(states[i].Gamepad.sThumbLY)/max;
		case RightStickX: return float(states[i].Gamepad.sThumbRX)/max;
		case RightStickY: return float(states[i].Gamepad.sThumbRY)/max;
		case A: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1.0f : 0.0f;
		case B: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1.0f : 0.0f;
		case X: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1.0f : 0.0f;
		case Y: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 1.0f : 0.0f;
		case LeftButton: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1.0f : 0.0f;
		case RightButton: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1.0f : 0.0f;
		case LeftTrigger: return float(states[i].Gamepad.bLeftTrigger)/255;
		case RightTrigger: return float(states[i].Gamepad.bRightTrigger)/255;
		case Start: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1.0f : 0.0f;
		case Back: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 1.0f : 0.0f;
		case DpadUp: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1.0f : 0.0f;
		case DpadDown: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1.0f : 0.0f;
		case DpadLeft: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1.0f : 0.0f;
		case DpadRight: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1.0f : 0.0f;
		case LeftStick: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1.0f : 0.0f;
		case RightStick: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1.0f : 0.0f;
		default: return 0.0f;
	}
}

bool XinputController::down(const controller::Button b) const {
	return state(b) > 0.5f;
}

} // namespace jngl
