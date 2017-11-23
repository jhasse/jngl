// Copyright 2012-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "input.hpp"

#include "../windowptr.hpp"

namespace jngl {
	void setKeyPressed(key::KeyType key, bool p) {
		pWindow->setKeyPressed(key, p);
	}

	void setKeyPressed(const std::string& key, bool p) {
		pWindow->setKeyPressed(key, p);
	}

	void setMousePressed(mouse::Button button, bool p) {
		pWindow->setMousePressed(button, p);
	}

	void setMouseDown(mouse::Button button, bool d) {
		pWindow->setMouseDown(button, d);
	}

	bool isMultitouch() {
		return pWindow->isMultitouch();
	}

	KeyboardType keyboardType = Default;

	void setKeyboardType(KeyboardType t) {
		keyboardType = t;
	}

	KeyboardType getKeyboardType() {
		return keyboardType;
	}

#if defined(ANDROID) || defined(IOS)
	bool isControllerConnected(int) {
		return false;
	}

	float getControllerState(int, controller::Button) {
		return 0.0f;
	}

	bool getControllerDown(int, controller::Button) {
		return false;
	}

	void setControllerVibration(int, unsigned short, unsigned short) {
	}
#endif
}
