// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Key class
/// @file
#pragma once

#include "input.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace jngl {

/// A key on the keyboard: one without a character, e.g. jngl::key::F1, or the one that types a
/// character, e.g. Q or Ö
///
/// Unlike for jngl::keyDown(const std::string&), Shift doesn't matter: `jngl::Key('q')` is the
/// same key as `jngl::Key('Q')`. That makes it something to store, e.g. the key the user has chosen
/// for an action, see toString() and jngl::keysPressed().
///
/// Example:
/// \code
/// jngl::Key jump = jngl::key::Space;
/// if (choosing) {
///     if (const auto keys = jngl::keysPressed(); !keys.empty()) {
///         jump = keys.front();
///         choosing = false;
///     }
/// } else if (jump.pressed()) {
///     player.jump();
/// }
/// \endcode
class Key {
public:
	/// A key without a character, e.g. jngl::key::F1, or key::Ctrl for either of the two
	Key(key::KeyType); // NOLINT

	/// The key that types \p character, e.g. 'q' (the same as 'Q'), or key::Space for ' '
	///
	/// \throws std::runtime_error if \p character isn't ASCII
	Key(char character); // NOLINT

	/// The key that types \p character, which has to be exactly one UTF-8 character, e.g. "ö" (the
	/// same as "Ö"), or key::Space for " "
	///
	/// \throws std::runtime_error if \p character isn't exactly one character
	explicit Key(std::string character);

	/// Used to display the key, e.g. "F1", "Space" or "Ö", which fromString() turns back into it
	///
	/// A key without a character gets jngl::keyToString(), one with a letter it in uppercase.
	std::string toString() const;

	/// The key whose toString() is \p name, or std::nullopt if there's none
	static std::optional<Key> fromString(std::string_view name);

	/// Whether the key is down, regardless of Shift, Ctrl and Alt
	bool down() const;

	/// Whether the key has been pressed since the next to last call to updateInput(), regardless of
	/// Shift, Ctrl and Alt
	bool pressed() const;

	bool operator==(const Key&) const = default;

private:
	/// A character as it's typed without Shift, e.g. "q"
	std::variant<key::KeyType, std::string> key;
};

/// The keys that have been pressed since the next to last call to updateInput(), in the order they
/// went down, e.g. to let the user choose one for an action
///
/// Keys which a jngl::Key can't be, e.g. the numpad's, are left out. A key that's held down for a
/// while is pressed again and again, as the operating system repeats it.
std::vector<Key> keysPressed();

} // namespace jngl
