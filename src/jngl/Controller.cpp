// Copyright 2018-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Controller.hpp"

#include "../windowptr.hpp"

namespace jngl {

bool Controller::pressed(const controller::Button button) {
	if (buttonPressed[button] == ButtonState::UNKNOWN && down(button)) {
		buttonPressed[button] = ButtonState::PRESSED;
		pWindow->addUpdateInputCallback([self = shared_from_this(), button]() {
			self->buttonPressed[button] = ButtonState::WAITING_FOR_UP;
		});
	}
	if (buttonPressed[button] == ButtonState::WAITING_FOR_UP && !down(button)) {
		// We're using addUpdateInputCallback since we don't want to change the state during a frame
		pWindow->addUpdateInputCallback([self = shared_from_this(), button]() {
			self->buttonPressed[button] = ButtonState::UNKNOWN;
		});
	}
	return buttonPressed[button] == ButtonState::PRESSED;
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

void Controller::rumble(float, std::chrono::milliseconds) {
}

} // namespace jngl
