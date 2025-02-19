// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "KeyboardShortcut.hpp"

namespace jngl {

KeyboardShortcut::KeyboardShortcut() : key('\0') {
}

KeyboardShortcut::KeyboardShortcut(key::KeyType modifier, char key)
: modifiers({
      modifier,
  }),
  key(key) {
}
KeyboardShortcut::KeyboardShortcut(key::KeyType modifier1, key::KeyType modifier2, char key)
: modifiers({
      modifier1,
      modifier2,
  }),
  key(key) {
}

KeyboardShortcut::KeyboardShortcut(key::KeyType key) : key(key) {
}

std::string KeyboardShortcut::toString() const {
	std::string result;
	for (const auto modifier : modifiers) {
		result += keyToString(modifier) + "+";
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

} // namespace jngl
