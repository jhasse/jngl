// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "AppleController.h"

namespace jngl {

AppleController::AppleController(GCController* gc) : gc(gc) {
}

bool AppleController::down(const controller::Button button) const {
	return state(button) > 0.5f;
}

float AppleController::stateImpl(const controller::Button button) const {
	switch (button) {
	case controller::A: return gc.extendedGamepad.buttonA.value;
	case controller::B: return gc.extendedGamepad.buttonB.value;
	case controller::X: return gc.extendedGamepad.buttonX.value;
	case controller::Y: return gc.extendedGamepad.buttonY.value;
	case controller::LeftTrigger: return gc.extendedGamepad.leftTrigger.value;
	case controller::RightTrigger: return gc.extendedGamepad.rightTrigger.value;
	case controller::LeftStickX: return gc.extendedGamepad.leftThumbstick.xAxis.value;
	case controller::LeftStickY: return gc.extendedGamepad.leftThumbstick.yAxis.value;
	case controller::RightStickX: return gc.extendedGamepad.rightThumbstick.xAxis.value;
	case controller::RightStickY: return gc.extendedGamepad.rightThumbstick.yAxis.value;
	case controller::DpadLeft: return gc.extendedGamepad.dpad.left.value;
	case controller::DpadRight: return gc.extendedGamepad.dpad.right.value;
	case controller::DpadUp: return gc.extendedGamepad.dpad.up.value;
	case controller::DpadDown: return gc.extendedGamepad.dpad.down.value;
	case controller::LeftButton: return gc.extendedGamepad.leftShoulder.value;
	case controller::RightButton: return gc.extendedGamepad.rightShoulder.value;
	case controller::LeftStick: return gc.extendedGamepad.leftThumbstickButton.value;
	case controller::RightStick: return gc.extendedGamepad.rightThumbstickButton.value;
	case controller::Start:
		if (@available(iOS 13, *)) {
			return gc.extendedGamepad.buttonOptions.value;
		}
		return 0;
	case controller::Back:
		if (@available(iOS 13, *)) {
			return gc.extendedGamepad.buttonMenu.value;
		}
		return 0;
	default:
		return 0;
	}
}

} // namespace jngl
