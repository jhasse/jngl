// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "Key.hpp"

#include "../helper.hpp"
#include "../windowptr.hpp"

#include <stdexcept>

namespace jngl {

Key::Key(const key::KeyType key) : key(key) {
}

Key::Key(const char character) : Key(std::string(1, character)) {
}

Key::Key(std::string character) : key(key::Space) {
	if (!isOneCharacter(character)) {
		throw std::runtime_error("A Key's character has to be exactly one UTF-8 character.");
	}
	if (character != " ") {
		key = withoutShift(std::move(character));
	}
}

std::string Key::toString() const {
	if (const auto* const type = std::get_if<key::KeyType>(&key)) {
		return keyToString(*type);
	}
	return withShift(std::get<std::string>(key));
}

std::optional<Key> Key::fromString(const std::string_view name) {
	for (int i = 0; i <= key::Any; ++i) {
		if (const auto type = static_cast<key::KeyType>(i); keyToString(type) == name) {
			return type;
		}
	}
	if (isOneCharacter(name)) {
		return Key(std::string(name));
	}
	return std::nullopt;
}

bool Key::down() const {
	if (const auto* const type = std::get_if<key::KeyType>(&key)) {
		return keyDown(*type);
	}
	const auto& character = std::get<std::string>(key);
	return pWindow->getKeyDown(character) || pWindow->getKeyDown(withShift(character));
}

bool Key::pressed() const {
	if (const auto* const type = std::get_if<key::KeyType>(&key)) {
		return keyPressed(*type);
	}
	const auto& character = std::get<std::string>(key);
	return pWindow->getKeyPressed(character) || pWindow->getKeyPressed(withShift(character));
}

std::vector<Key> keysPressed() {
	return pWindow->getKeysPressed();
}

} // namespace jngl
