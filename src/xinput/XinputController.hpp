// Copyright 2018-2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "../jngl/Controller.hpp"

#include <atomic>
#include <condition_variable>
#include <optional>
#include <thread>
#include <windows.h>
#include <xinput.h>

namespace jngl {

extern XINPUT_STATE states[XUSER_MAX_COUNT];

class XinputController : public jngl::Controller {
public:
	explicit XinputController(int number);
	~XinputController();
	float stateImpl(controller::Button) const override;
	bool down(controller::Button) const override;
	void rumble(float, std::chrono::milliseconds) override;

private:
	int i;
	std::thread thread;
	std::condition_variable cv;
	std::mutex mutex;
	std::optional<std::chrono::milliseconds> rumbleDuration;
	bool stopRequested = false;
};

} // namespace jngl
