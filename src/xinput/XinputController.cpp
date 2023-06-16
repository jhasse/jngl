// Copyright 2018-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "XinputController.hpp"

#include "../window.hpp"

namespace jngl {

XINPUT_STATE states[XUSER_MAX_COUNT];

XinputController::XinputController(const int number) : i(number), thread([this]() {
	std::unique_lock<std::mutex> lock(mutex); // to access rumbleDuration
	while (!stopRequested) {
		if (rumbleDuration) {
			auto duration = *rumbleDuration;
			rumbleDuration = std::nullopt;
			cv.wait_for(lock, duration); // unlocks the mutex while waiting
			if (stopRequested || rumbleDuration) continue;
			XINPUT_VIBRATION vibration{};
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
			XInputSetState(i, &vibration);
		} else {
			cv.wait(lock);
		}
	}
}) {
}

XinputController::~XinputController() {
	stopRequested = true;
	cv.notify_one();
	thread.join();
}

float XinputController::stateImpl(const controller::Button b) const {
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

void XinputController::rumble(const float amount, std::chrono::milliseconds time) {
	std::unique_lock<std::mutex> lock(mutex);
	XINPUT_VIBRATION vibration{};
	vibration.wLeftMotorSpeed = static_cast<WORD>(amount * 65535);
	vibration.wRightMotorSpeed = static_cast<WORD>(amount * 65535);
	XInputSetState(i, &vibration);
	rumbleDuration = time;
	cv.notify_one();
}

std::vector<std::shared_ptr<Controller>> getConnectedControllers() {
	static std::shared_ptr<XinputController> controllers[XUSER_MAX_COUNT];
	std::vector<std::shared_ptr<Controller>> tmp;
	for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		DWORD dwResult = XInputGetState(i, &state);
		if (dwResult == ERROR_SUCCESS) {
			if (!controllers[i]) {
				controllers[i] = std::make_shared<XinputController>(i);
			}
			tmp.push_back(controllers[i]);
		}
	}
	return tmp;
}

void calculateStick(short& x, short& y, int deadzone) {
	float magnitude = float(std::sqrt(x * x + y * y));
	float normX = x / magnitude;
	float normY = y / magnitude;

	const int max = 32767;

	if (magnitude > deadzone) {
		if (magnitude > max) magnitude = max;
		magnitude -= deadzone;
		x = short(max * normX * magnitude / (max - deadzone));
		y = short(max * normY * magnitude / (max - deadzone));
	} else {
		x = y = 0;
	}
}

void calculateTrigger(BYTE& v) {
	if (v > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		v -= XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
		v = BYTE(255 * v / float(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD));
	} else {
		v = 0;
	}
}

void Window::updateControllerStates() {
	static std::array<bool, XUSER_MAX_COUNT> controllersConnected;
	const auto lastControllersConnected = controllersConnected;
	for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
		DWORD result = XInputGetState(i, &states[i]);
		if (result == ERROR_SUCCESS) {
			controllersConnected[i] = true;
			calculateStick(states[i].Gamepad.sThumbLX, states[i].Gamepad.sThumbLY,
			               XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			calculateStick(states[i].Gamepad.sThumbRX, states[i].Gamepad.sThumbRY,
			               XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			calculateTrigger(states[i].Gamepad.bLeftTrigger);
			calculateTrigger(states[i].Gamepad.bRightTrigger);
		} else {
			controllersConnected[i] = false;
		}
	}
	if (controllerChangedCallback && lastControllersConnected != controllersConnected) {
		controllerChangedCallback();
	}
}

} // namespace jngl
