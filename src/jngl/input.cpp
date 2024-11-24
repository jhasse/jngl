// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

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

std::vector<Vec2> getTouchPositions() {
	if (mouseDown(mouse::Left)) {
		if (isMultitouch()) {
			return pWindow->getTouchPositions();
		}
		return { getMousePos() };
	}
	return {};
}

KeyboardType keyboardType = Default;

void setKeyboardType(KeyboardType t) {
	keyboardType = t;
}

KeyboardType getKeyboardType() {
	return keyboardType;
}

void onControllerChanged(std::function<void()> callback) {
	pWindow->onControllerChanged(std::move(callback));
}

std::string getTextInput() {
	return pWindow->getTextInput();
}

Finally hideMouse() {
	pWindow->increaseMouseHiddenCount();
	return Finally([] { pWindow->decreaseMouseHiddenCount(); });
}

} // namespace jngl
