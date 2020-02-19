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

float AndroidController::stateImpl(const controller::Button button) const {
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

} // namespace jngl
