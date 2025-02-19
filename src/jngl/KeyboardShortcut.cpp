// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "KeyboardShortcut.hpp"

namespace jngl {

KeyboardShortcut::KeyboardShortcut() : key('\0') {
}

KeyboardShortcut::KeyboardShortcut(key::KeyType modifier, char key) : key(key) {
	switch (modifier) {
	case key::Ctrl:
		ctrl = true;
		break;
	case key::Alt:
		alt = true;
		break;
	case key::Shift:
		shift = true;
		break;
	default:
		throw std::runtime_error("Only Ctrl, Shift or Alt are allowed as modifiers");
	}
}
KeyboardShortcut::KeyboardShortcut(key::KeyType modifier1, key::KeyType modifier2, char key)
: key(key) {
	switch (modifier1) {
	case key::Ctrl:
		ctrl = true;
		break;
	case key::Alt:
		alt = true;
		break;
	case key::Shift:
		shift = true;
		break;
	default:
		throw std::runtime_error("Only Ctrl, Shift or Alt are allowed as modifiers");
	}
	switch (modifier2) {
	case key::Ctrl:
		if (ctrl) {
			throw std::runtime_error("Ctrl can only be used once");
		}
		ctrl = true;
		break;
	case key::Alt:
		if (alt) {
			throw std::runtime_error("Alt can only be used once");
		}
		alt = true;
		break;
	case key::Shift:
		if (shift) {
			throw std::runtime_error("Shift can only be used once");
		}
		shift = true;
		break;
	default:
		throw std::runtime_error("Only Ctrl, Shift or Alt are allowed as modifiers");
	}
}

KeyboardShortcut::KeyboardShortcut(key::KeyType key) : key(key) {
}

std::string KeyboardShortcut::toString() const {
	std::string result;
	if (ctrl) {
		result += "Ctrl+";
	}
	if (shift) {
		result += "Shift+";
	}
	if (alt) {
		result += "Alt+";
	}
	if (std::holds_alternative<key::KeyType>(key)) {
		result += keyToString(std::get<key::KeyType>(key));
	} else {
		char k = std::get<char>(key);
		if (k == '\0') {
			return result;
		}
		result += static_cast<char>(std::toupper(k));
	}
	return result;
}

bool KeyboardShortcut::pressed() const {
	if (std::holds_alternative<key::KeyType>(key)) {
		if (!keyPressed(std::get<key::KeyType>(key))) {
			return false;
		}
	} else {
		char k = std::get<char>(key);
		if (k == '\0') {
			return false;
		}
		if (!keyPressed(k)) {
			return false;
		}
	}
	return keyDown(jngl::key::Ctrl) == ctrl && keyDown(jngl::key::Alt) == alt &&
	       keyDown(jngl::key::Shift) == shift;
}

} // namespace jngl
