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
		default: return false;
	}
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
