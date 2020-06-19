// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "AndroidController.hpp"

#include <cmath>

namespace jngl {

AndroidController::AndroidController() = default;

AndroidController::~AndroidController() = default;

bool AndroidController::down(const controller::Button button) const {
	switch (button) {
		case controller::A: return buttonA;
		case controller::B: return buttonB;
		case controller::Start: return buttonStart;
		default: return state(button) > 0.5f;
	}
}

float AndroidController::stateWithoutDeadzone(const controller::Button button) const {
	switch (button) {
		case controller::LeftStickX: return leftStickX;
		case controller::LeftStickY: return -leftStickY;
		case controller::RightStickX: return rightStickX;
		case controller::RightStickY: return -rightStickY;
		case controller::DpadLeft: return std::clamp(-dpadX, 0.f, 1.f);
		case controller::DpadRight: return std::clamp(dpadX, 0.f, 1.f);
		case controller::DpadUp: return std::clamp(-dpadY, 0.f, 1.f);
		case controller::DpadDown: return std::clamp(dpadY, 0.f, 1.f);
		default: return 0;
	}
}

float AndroidController::stateImpl(controller::Button button) const {
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
	// Android gives sticks as square coordinates
	const double tmp = state * std::sqrt(1 - 0.5 * otherState * otherState);
	otherState *= std::sqrt(1 - 0.5 * state * state);
	state = static_cast<float>(tmp);
	if (state * state + otherState * otherState < 0.1) { // inside deadzone circle?
		return 0;
	}
	return state;
}

} // namespace jngl
