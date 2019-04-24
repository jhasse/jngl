// Copyright 2018-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Controller.hpp"

#include "../windowptr.hpp"

namespace jngl {

bool Controller::pressed(const controller::Button button) {
	if (buttonPressed[button] && !down(button)) {
		pWindow->addUpdateInputCallback([this, button]() { buttonPressed[button] = false; });
	} else if (down(button) && !buttonPressed[button]) {
		buttonPressed[button] = true;
		return true;
	}
	return false;
}

float Controller::state(const controller::Button button) const {
	if (button == controller::LeftStickXInverse) {
		return -stateImpl(controller::LeftStickX);
	}
	if (button == controller::LeftStickYInverse) {
		return -stateImpl(controller::LeftStickY);
	}
	if (button == controller::RightStickXInverse) {
		return -stateImpl(controller::RightStickX);
	}
	if (button == controller::RightStickYInverse) {
		return -stateImpl(controller::RightStickY);
	}
	return stateImpl(button);
}

} // namespace jngl
