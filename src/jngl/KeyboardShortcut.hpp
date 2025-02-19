// Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::KeyboardShortcut class
/// @file
#pragma once

#include "input.hpp"

#include <variant>

namespace jngl {

class KeyboardShortcut {
public:
	/// No shortcut, will never trigger
	KeyboardShortcut();

	/// Modifier + key, e.g. { jngl::key::Control, 's' } for Ctrl+S
	///
	/// \throws std::runtime_error if modifier is not key::Ctrl, key::Shift or key::Alt
	KeyboardShortcut(key::KeyType modifier, char key);

	/// Two modifiers + key, e.g. { jngl::key::Control, jngl::key::Shift, 's' } for Ctrl+Shift+S
	///
	/// \throws std::runtime_error if modifier is not key::Ctrl, key::Shift or key::Alt or if a modifier is used
	/// twice
	KeyboardShortcut(key::KeyType modifier1, key::KeyType modifier2, char key);

	/// Single key, e.g. jngl::key::F1
	KeyboardShortcut(key::KeyType key); // NOLINT

	/// Used to display the shortcut, e.g. "Ctrl+Shift+S" or "F3", returns "" for no shortcut
	///
	/// \note The order of modifiers is always Ctrl, Shift, Alt
	std::string toString() const;

	/// Returns true if all modifiers and the key of the shortcut was pressed in this step
	///
	/// \note Won't return true for Ctrl+S if Ctrl+Shift+S or Ctrl+Alt+S was pressed.
	bool pressed() const;

private:
	bool ctrl = false;
	bool shift = false;
	bool alt = false;
	std::variant<key::KeyType, char> key;
};

} // namespace jngl
