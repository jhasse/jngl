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
		return "Left";
	case key::Up:
		return "Up";
	case key::Right:
		return "Right";
	case key::Down:
		return "Down";
	case key::PageUp:
		return "PageUp";
	case key::PageDown:
		return "PageDown";
	case key::Home:
		return "Home";
	case key::End:
		return "End";
	case key::BackSpace:
		return "BackSpace";
	case key::Tab:
		return "Tab";
	case key::Clear:
		return "Clear";
	case key::Return:
		return "Return";
	case key::Pause:
		return "Pause";
	case key::Escape:
		return "Escape";
	case key::Delete:
		return "Delete";
	case key::ControlL:
		return "CtrlL";
	case key::ControlR:
		return "CtrlR";
	case key::CapsLock:
		return "CapsLock";
	case key::AltL:
		return "Alt";
	case key::AltR:
		return "AltGr";
	case key::SuperL:
		return "SuperL";
	case key::SuperR:
		return "SuperR";
	case key::Space:
		return "Space";
	case key::ShiftL:
		return "ShiftL";
	case key::ShiftR:
		return "ShiftR";
	case key::Any:
		return "Any";
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
