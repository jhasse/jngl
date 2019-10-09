// Copyright 2018-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "../jngl/Controller.hpp"

#include <windows.h>
#include <xinput.h>

namespace jngl {

extern XINPUT_STATE states[XUSER_MAX_COUNT];

class XinputController : public jngl::Controller {
public:
	JNGLDLL_API XinputController(int number);
	float stateImpl(controller::Button) const override;
	bool down(controller::Button) const override;
	void setVibration(float) override;

private:
	int i;
};

} // namespace jngl
