// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../jngl/Controller.hpp"

namespace jngl {

class AndroidController : public Controller {
public:
	AndroidController();
	AndroidController(const AndroidController&) = delete;
	AndroidController& operator=(const AndroidController&) = delete;
	AndroidController(AndroidController&&) = delete;
	AndroidController& operator=(AndroidController&&) = delete;
	~AndroidController() override;

	bool down(controller::Button) const override;
	float stateImpl(controller::Button) const override;

	float leftStickX = 0;
	float leftStickY = 0;
	float rightStickX = 0;
	float rightStickY = 0;
	float dpadX = 0;
	float dpadY = 0;
	bool buttonA = false;
	bool buttonB = false;
	bool buttonStart = false;

private:
	float stateWithoutDeadzone(controller::Button) const;
};

} // namespace jngl
