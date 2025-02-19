// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::KeyboardShortcut class
/// @file
#pragma once

#include "input.hpp"

namespace jngl {

class KeyboardShortcut {
public:
	/// no shortcut, will never trigger
	KeyboardShortcut();

	/// modifier + key, e.g. { jngl::key::Control, 's' } for Ctrl+S
	KeyboardShortcut(key::KeyType modifier, char key);

	/// two modifiers + key, e.g. { jngl::key::Control, jngl::key::Shift, 's' } for Ctrl+Shift+S
	KeyboardShortcut(key::KeyType modifier1, key::KeyType modifier2, char key);

	// single key, e.g. jngl::key::F1
	KeyboardShortcut(key::KeyType key); // NOLINT

	std::string toString() const;

private:
	std::vector<key::KeyType> modifiers;
	std::variant<key::KeyType, char> key;
};

} // namespace jngl
