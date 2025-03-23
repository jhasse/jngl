// Copyright 2017-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "SdlController.hpp"

#include "../../log.hpp"
#include "../../window.hpp"

#include <cmath>

namespace jngl {

SdlController::SdlController(SDL_Joystick* const handle, const SDL_JoystickID joystickId)
: handle(handle), haptic(SDL_OpenHapticFromJoystick(handle)) {
	internal::debug("Controller #{}: {}", joystickId, SDL_GetJoystickName(handle));
	internal::trace("Axes: {}, Balls: {}, Buttons: {}, Hats: {}", SDL_GetNumJoystickAxes(handle),
	                SDL_GetNumJoystickBalls(handle), SDL_GetNumJoystickButtons(handle),
	                SDL_GetNumJoystickHats(handle));
	if (haptic) {
		if (!SDL_InitHapticRumble(haptic)) {
			internal::error(SDL_GetError());
		}
	} else {
		internal::trace(SDL_GetError());
	}
	switch (SDL_GetNumJoystickButtons(handle)) {
		case 11:
			model = Model::XBOX_WIRED;
			break;
		case 13:
			model = Model::DS4;
			break;
		case 15:
			model = Model::XBOX;
			break;
		default: {
			if (SDL_IsGamepad(joystickId)) {
				gameController = SDL_OpenGamepad(joystickId);
				if (gameController) {
					this->handle = nullptr;
					SDL_CloseJoystick(handle);
				} else {
					internal::warn("SDL_OpenGamepad failed, falling back to joystick.");
				}
			}
		}
	};
}

SdlController::~SdlController() {
	if (handle) {
		SDL_CloseJoystick(handle);
	} else {
		SDL_CloseGamepad(gameController);
	}
}

float SdlController::stateWithoutDeadzone(controller::Button button) const {
	if (handle) {
		const bool xboxWired = model == Model::XBOX_WIRED;
		int axisIndex;
		switch (button) {
			case controller::LeftStickX: axisIndex = 0; break;
			case controller::LeftStickY: axisIndex = 1; break;
			case controller::RightStickX: axisIndex = xboxWired ? 2 : 3; break;
			case controller::RightStickY: axisIndex = xboxWired ? 3 : 4; break;
			case controller::LeftTrigger: axisIndex = xboxWired ? 5 : 2; break;
			case controller::RightTrigger: axisIndex = xboxWired ? 4 : 5; break;
			default: return down(button) ? 1 : 0;
		}
		float state = SDL_GetJoystickAxis(handle, axisIndex);
		if (state < 0) {
			state /= 32768;
		} else {
			state /= 32767;
		}
		if (button == controller::LeftStickY || button == controller::RightStickY) {
			state *= -1;
		}
		return state;
	}
	SDL_GamepadAxis axis;
	switch (button) {
		case controller::LeftStickX: axis = SDL_GAMEPAD_AXIS_LEFTX; break;
		case controller::LeftStickY: axis = SDL_GAMEPAD_AXIS_LEFTY; break;
		case controller::RightStickX: axis = SDL_GAMEPAD_AXIS_RIGHTX; break;
		case controller::RightStickY: axis = SDL_GAMEPAD_AXIS_RIGHTY; break;
		case controller::LeftTrigger: axis = SDL_GAMEPAD_AXIS_LEFT_TRIGGER; break;
		case controller::RightTrigger: axis = SDL_GAMEPAD_AXIS_RIGHT_TRIGGER; break;
		default: return down(button) ? 1 : 0;
	}
	float state = SDL_GetGamepadAxis(gameController, axis);
	if (state < 0) {
		state /= 32768;
	} else {
		state /= 32767;
	}
	if (button == controller::LeftStickY || button == controller::RightStickY) {
		state *= -1;
	}
	return state;
}

float SdlController::stateImpl(controller::Button button) const {
	float state = stateWithoutDeadzone(button);
	controller::Button otherAxis;
	switch(button) {
		case controller::LeftStickY:
			otherAxis = controller::LeftStickX;
			break;
		case controller::LeftStickX:
			otherAxis = controller::LeftStickY;
			break;
		case controller::RightStickY:
			otherAxis = controller::RightStickX;
			break;
		case controller::RightStickX:
			otherAxis = controller::RightStickY;
			break;
		default:
			return state; // no deadzone needed
	}
	auto otherState = stateWithoutDeadzone(otherAxis);
	if (model == Model::DS4) { // DualShock 4 returns sticks as square coordinates
		const double tmp = state * std::sqrt(1 - 0.5 * otherState * otherState);
		otherState *= std::sqrt(1 - 0.5 * state * state);
		state = static_cast<float>(tmp);
	}
	if (!gameController && model != Model::XBOX_WIRED && model != Model::XBOX) {
		return state; // no deadzone needed
	}
	const auto lengthSquared = state * state + otherState * otherState;
	const float DEADZONE_RADIUS = 0.2;
	if (lengthSquared < DEADZONE_RADIUS * DEADZONE_RADIUS) { // inside deadzone circle?
		return 0;
	}

	// smooth out transition out of deadzone to 2 * deadzone:
	if (lengthSquared < DEADZONE_RADIUS * 2 * DEADZONE_RADIUS * 2) {
		float directionVectorInDeadzone = state / std::sqrt(lengthSquared) * DEADZONE_RADIUS;
		return (state - directionVectorInDeadzone) * 2;
	}

	// not a perfect square, but most controllers are returning coordinates outside of a circle:
	if (lengthSquared > 1) {
		return state / std::sqrt(lengthSquared);
	}

	return state;
}

bool SdlController::down(const controller::Button button) const {
	if (handle) {
		const bool xbox = (model == Model::XBOX || model == Model::XBOX_WIRED);
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
			case controller::DpadUp: return (SDL_GetJoystickHat(handle, 0) & SDL_HAT_UP) != 0;
			case controller::DpadDown: return (SDL_GetJoystickHat(handle, 0) & SDL_HAT_DOWN) != 0;
			case controller::DpadLeft: return (SDL_GetJoystickHat(handle, 0) & SDL_HAT_LEFT) != 0;
			case controller::DpadRight: return (SDL_GetJoystickHat(handle, 0) & SDL_HAT_RIGHT) != 0;
			case controller::LeftStick: buttonIndex = xbox ? 9 : 11; break;
			case controller::RightStick: buttonIndex = xbox ? 10 : 12; break;
			default: return state(button) > 0.5f; // e.g. LeftStickX
		}
		return SDL_GetJoystickButton(handle, buttonIndex);
	}
	switch (button) {
	case controller::Button::A:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_SOUTH);
	case controller::Button::B:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_EAST);
	case controller::Button::X:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_WEST);
	case controller::Button::Y:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_NORTH);
	case controller::Button::LeftButton:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
	case controller::Button::RightButton:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
	case controller::Button::Start:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_START);
	case controller::Button::Back:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_BACK);
	case controller::Button::DpadUp:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_DPAD_UP);
	case controller::Button::DpadDown:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
	case controller::Button::DpadLeft:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
	case controller::Button::DpadRight:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
	case controller::Button::LeftStick:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_LEFT_STICK);
	case controller::Button::RightStick:
		return SDL_GetGamepadButton(gameController, SDL_GAMEPAD_BUTTON_RIGHT_STICK);
	default:
		return state(button) > 0.5f;
	}
}

void SdlController::rumble(const float vibration, const std::chrono::milliseconds ms) {
	if (haptic) {
		SDL_PlayHapticRumble(haptic, vibration, ms.count());
	} else if (handle) {
		SDL_RumbleJoystick(handle, static_cast<Uint16>(vibration * 65535),
		                   static_cast<Uint16>(vibration * 65535), ms.count());
	} else {
		SDL_RumbleGamepad(gameController, static_cast<Uint16>(vibration * 65535),
		                  static_cast<Uint16>(vibration * 65535), ms.count());
	}
}

bool SdlController::is(SDL_Joystick* const handle) const {
	if (this->handle) {
		return this->handle == handle;
	}
	return SDL_GetGamepadJoystick(gameController) == handle;
}

std::vector<std::shared_ptr<Controller>> getConnectedControllers() {
	static std::vector<std::shared_ptr<SdlController>> controllers;
	int numJoysticks = 0;
	SDL_JoystickID* joystickId = SDL_GetJoysticks(&numJoysticks);
	std::vector<std::shared_ptr<Controller>> rtn;
	for (int i = 0; i < numJoysticks; ++i) {
		SDL_Joystick* handle = SDL_OpenJoystick(i);
		if (SDL_GetNumJoystickButtons(handle) == 0 ||
		    (SDL_GetNumJoystickAxes(handle) < 2 && SDL_GetNumJoystickBalls(handle) == 0 &&
		     SDL_GetNumJoystickHats(handle) < 2)) {
			// This could be the Motion Sensors of the DS4 or "Keychron K3 Pro System Control".
			// Ignore it:
			SDL_CloseJoystick(handle);
			continue;
		}
		bool found = false;
		for (const auto& controller : controllers) {
			if (controller->is(handle)) {
				SDL_CloseJoystick(handle);
				rtn.push_back(controller);
				found = true;
				break;
			}
		}
		if (!found) {
			controllers.emplace_back(std::make_shared<SdlController>(handle, joystickId[i]));
			rtn.emplace_back(controllers.back());
		}
	}
	return rtn;
}

void Window::updateControllerStates() {
}

} // namespace jngl
