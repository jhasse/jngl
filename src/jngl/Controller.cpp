// Copyright 2018-2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Controller.hpp"

#include "../windowptr.hpp"

namespace jngl {

bool Controller::pressed(const controller button) {
	if (buttonPressed[static_cast<int>(button)] == ButtonState::UNKNOWN && down(button)) {
		buttonPressed[static_cast<int>(button)] = ButtonState::PRESSED;
		pWindow->addUpdateInputCallback([self = shared_from_this(), button]() {
			self->buttonPressed[static_cast<int>(button)] = ButtonState::WAITING_FOR_UP;
		});
	}
	if (buttonPressed[static_cast<int>(button)] == ButtonState::WAITING_FOR_UP && !down(button)) {
		// We're using addUpdateInputCallback since we don't want to change the state during a frame
		pWindow->addUpdateInputCallback([self = shared_from_this(), button]() {
			self->buttonPressed[static_cast<int>(button)] = ButtonState::UNKNOWN;
		});
	}
	return buttonPressed[static_cast<int>(button)] == ButtonState::PRESSED;
}

float Controller::state(const controller button) const {
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
