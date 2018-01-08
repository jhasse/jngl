// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "../jngl/Controller.hpp"

namespace jngl {

class XinputController : public jngl::Controller {
public:
	XinputController(int number);
	float state(controller::Button) const override;
	bool down(controller::Button) const override;

private:
	int i;
};

} // namespace jngl
