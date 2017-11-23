#include "Controller.hpp"

#include "../windowptr.hpp"

namespace jngl {

bool Controller::pressed(const controller::Button button) {
	if (buttonPressed[button] && !down(button)) {
		pWindow->addUpdateInputCallback([=]() {
			buttonPressed[button] = false;
		});
		return true;
	} else if (!down(button)) {
		buttonPressed[button] = true;
	}
	return false;
}

} // namepace jngl
