// Copyright 2012-2017 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/input.hpp"
#include "sdl.hpp"
#include "../jngl/debug.hpp"
#include "SdlController.hpp"

#include <map>

namespace jngl {
	void setKeyboardVisible(bool) {
	}

	std::map<int, SDL_Joystick*> joysticks;


	std::vector<std::shared_ptr<Controller>> getConnectedControllers() {
		static std::vector<std::shared_ptr<SdlController>> controllers;
		int numJoysticks = SDL_NumJoysticks();
		std::vector<std::shared_ptr<Controller>> rtn;
		for (int i = 0; i < numJoysticks; ++i) {
			SDL_Joystick* handle = SDL_JoystickOpen(i);
			bool found = false;
			for (const auto& controller : controllers) {
				if (controller->is(handle)) {
					rtn.push_back(controller);
					found = true;
					break;
				}
			}
			if (!found) {
				controllers.emplace_back(std::make_shared<SdlController>(handle));
				rtn.emplace_back(controllers.back());
			}
		}
		return rtn;
	}

	bool isControllerConnected(const int number) {
		const auto it = joysticks.find(number);
		if (it == joysticks.end()) {
			if (number >= SDL_NumJoysticks()) {
				return false;
			}
			joysticks[number] = SDL_JoystickOpen(number);

			jngl::debug("Name: ");
			jngl::debugLn(SDL_JoystickNameForIndex(number));
			jngl::debug("Number of Axes: ");
			jngl::debugLn(SDL_JoystickNumAxes(joysticks[number]));
			jngl::debug("Number of Buttons: ");
			jngl::debugLn(SDL_JoystickNumButtons(joysticks[number]));
			jngl::debug("Number of Balls: ");
			jngl::debugLn(SDL_JoystickNumBalls(joysticks[number]));
			jngl::debug("Number of Hats: ");
			jngl::debugLn(SDL_JoystickNumHats(joysticks[number]));

			return true;
		}
		return SDL_JoystickGetAttached(it->second);
	}
}
