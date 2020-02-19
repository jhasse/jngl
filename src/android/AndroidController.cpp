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
	}
	return false;
}

void AndroidController::rumble(const float /*vibration*/, const std::chrono::milliseconds) {
	// not implemented!
}

float AndroidController::stateImpl(const controller::Button button) const {
	switch (button) {
		case controller::LeftStickX: return leftStickX;
		case controller::LeftStickY: return -leftStickY;
		case controller::RightStickX: return rightStickX;
		case controller::RightStickY: return -rightStickY;
		default: return 0;
	}
}

} // namespace jngl
