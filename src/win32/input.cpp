/*
Copyright 2012-2013 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "../jngl/input.hpp"
#include "../window.hpp"

namespace jngl {
	void setKeyboardVisible(bool) {
	}

	bool isControllerConnected(int number) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
    	DWORD dwResult = XInputGetState(number, &state);
        return dwResult == ERROR_SUCCESS;
	}

	float getControllerState(int i, controller::Button b) {
		const float max = 32767;

		using namespace controller;
		switch(b) {
			case LeftStickX: return float(states[i].Gamepad.sThumbLX)/max;
			case LeftStickY: return float(states[i].Gamepad.sThumbLY)/max;
			case RightStickX: return float(states[i].Gamepad.sThumbRX)/max;
			case RightStickY: return float(states[i].Gamepad.sThumbRY)/max;
			case A: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_A;
			case B: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_B;
			case X: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_X;
			case Y: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_Y;
			case LeftButton: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
			case RightButton: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
			case LeftTrigger: return float(states[i].Gamepad.bLeftTrigger)/255;
			case RightTrigger: return float(states[i].Gamepad.bRightTrigger)/255;
			case Start: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_START;
			case Back: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
			case DpadUp: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
			case DpadDown: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
			case DpadLeft: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
			case DpadRight: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
			case LeftStick: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
			case RightStick: return states[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
			default: return 0.0f;
		}
	}
}