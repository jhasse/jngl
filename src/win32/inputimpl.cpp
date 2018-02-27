// Copyright 2012-2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../jngl/input.hpp"

#include "XinputController.hpp"

#include <windows.h>
#include <xinput.h>

namespace jngl {

void setKeyboardVisible(bool) {
}

std::vector<std::shared_ptr<Controller>> getConnectedControllers() {
	static std::shared_ptr<XinputController> controllers[XUSER_MAX_COUNT];
	std::vector<std::shared_ptr<Controller>> tmp;
	for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		DWORD dwResult = XInputGetState(i, &state);
		if (dwResult == ERROR_SUCCESS) {
			if (!controllers[i]) {
				controllers[i] = std::make_shared<XinputController>(i);
			}
			tmp.push_back(controllers[i]);
		}
	}
	return tmp;
}

} // namespace jngl
