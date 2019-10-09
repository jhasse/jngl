// Copyright 2018-2019 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "../jngl/Controller.hpp"

#include <atomic>
#include <thread>
#include <windows.h>
#include <xinput.h>

namespace jngl {

extern XINPUT_STATE states[XUSER_MAX_COUNT];

class XinputController : public jngl::Controller {
public:
	JNGLDLL_API XinputController(int number);
	~XinputController();
	float stateImpl(controller::Button) const override;
	bool down(controller::Button) const override;
	void rumble(float, std::chrono::milliseconds) override;

private:
	int i;
	std::vector<std::pair<std::unique_ptr<std::atomic_bool>, std::thread>> threads;
};

} // namespace jngl
