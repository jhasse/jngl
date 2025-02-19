// Copyright 2012-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "../windowptr.hpp"

namespace jngl {

std::string keyToString(key::KeyType key) {
	switch (key) {
	case key::Ctrl:
		return "Ctrl";
	case key::Shift:
		return "Shift";
	case key::F1:
		return "F1";
	case key::F2:
		return "F2";
	case key::F3:
		return "F3";
	case key::F4:
		return "F4";
	case key::F5:
		return "F5";
	case key::F6:
		return "F6";
	case key::F7:
		return "F7";
	case key::F8:
		return "F8";
	case key::F9:
		return "F9";
	case key::F10:
		return "F10";
	case key::F11:
		return "F11";
	case key::F12:
		return "F12";
	case key::Left:
	case key::Up:
	case key::Right:
	case key::Down:
	case key::PageUp:
	case key::PageDown:
	case key::Home:
	case key::End:
	case key::BackSpace:
	case key::Tab:
	case key::Clear:
	case key::Return:
	case key::Pause:
	case key::Escape:
	case key::Delete:
	case key::ControlL:
	case key::ControlR:
	case key::CapsLock:
	case key::AltL:
	case key::AltR:
	case key::SuperL:
	case key::SuperR:
	case key::Space:
	case key::ShiftL:
	case key::ShiftR:
	case key::Any:
		break;
	}
	throw std::runtime_error("Unknown key");
}

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
