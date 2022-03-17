// Copyright 2017-2022 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "../../jngl/Controller.hpp"

#include "../sdl.hpp"

namespace jngl {

class SdlController : public Controller {
public:
	SdlController(SDL_Joystick*, int index);
	SdlController(const SdlController&) = delete;
	SdlController& operator=(const SdlController&) = delete;
	SdlController(SdlController&&) = delete;
	SdlController& operator=(SdlController&&) = delete;
	~SdlController() override;
	bool down(controller::Button) const override;
	void rumble(float, std::chrono::milliseconds) override;

	bool is(SDL_Joystick*) const;

private:
	float stateImpl(controller::Button) const override;
	float stateWithoutDeadzone(controller::Button) const;

	SDL_Joystick* handle;
	SDL_GameController* gameController = nullptr;
	SDL_Haptic* haptic = nullptr;

	enum class Model {
		XBOX,
		XBOX_WIRED,
		DS4,
		UNKNOWN,
	};
	Model model = Model::UNKNOWN;
};

} // namespace jngl
