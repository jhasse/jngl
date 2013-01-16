/*
Copyright 2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

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
}