// Copyright 2017-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "asndroidController.hpp"

#include "../jngl/debug.hpp"

#include <cmath>

namespace jngl {

AndroidController::AndroidController(){

}

AndroidController::~AndroidController() {

}

bool AndroidController::down(const controller::Button button) const {

	int buttonIndex;

	switch (button) {
			case controller::A: buttonIndex = 0; break;
			case controller::B: buttonIndex = 1; break;
	}


}

void AndroidController::rumble(const float vibration, const std::chrono::milliseconds ms) {
	// not implemented!
}

} // namespace jngl
