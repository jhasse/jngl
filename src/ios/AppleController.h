// Copyright 2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../jngl/Controller.hpp"

#import <GameController/GameController.h>

namespace jngl {

/// Apple iOS/tvOS Game Controller (should work on macOS, too)
class AppleController : public Controller {
public:
	AppleController(GCController*);

	bool down(controller::Button) const override;
	float stateImpl(controller::Button) const override;

private:
	GCController* gc;
};

} // namespace jngl
