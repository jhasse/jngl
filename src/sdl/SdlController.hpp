// Copyright 2017 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/Controller.hpp"

#include <SDL.h>

namespace jngl {

class SdlController : public Controller {
public:
	SdlController(SDL_Joystick*);
	~SdlController();
	float state(controller::Button) const override;
	bool down(controller::Button) const override;

	bool is(SDL_Joystick*) const;

private:
	SDL_Joystick* const handle;
};

} // namespace jngl
