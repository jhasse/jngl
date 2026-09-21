// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::TextInputSession class
/// @file
#pragma once

#include "Rect.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace jngl {

namespace internal {
void feedTextInput(const std::string& utf8);
void reapplyTextInputSession();
void reapplyTextInputArea();
void ensureLegacyTextInputStarted();
} // namespace internal

/// What kind of text a TextInputSession is used for
///
/// Used as a hint for on-screen keyboards and IMEs, e.g. to hide typed characters or show a
/// numeric keypad.
enum class TextInputType : uint8_t {
	Text,
	Password,
	Number,
	Email,
};

/// RAII handle representing a text input field that currently wants to receive typed characters
///
/// Text input is a mode with a lifetime: while at least one TextInputSession is alive, the OS/IME
/// is told to deliver typed characters (and, on Android and iOS, the on-screen keyboard is
/// shown). Construct one when a field gains focus and destroy it (e.g. by resetting an
/// `std::optional<TextInputSession>`) when it loses focus again.
///
/// Multiple instances can be alive at once, for example when a dialog with its own text field
/// opens on top of a form. The most recently constructed instance is the one that receives typed
/// characters; when it is destroyed, the instance that was active before it (if any) resumes
/// receiving characters. This also covers focus moving directly from one field to another: it
/// doesn't matter whether the newly focused field constructs its session before or after the
/// previously focused field destroys its own.
///
/// Example:
/// \code
/// class TextField {
/// public:
/// 	void setFocus(bool focus) {
/// 		if (focus) {
/// 			session.emplace();
/// 		} else {
/// 			session.reset();
/// 		}
/// 	}
///
/// 	void step() {
/// 		if (session) {
/// 			text += session->take();
/// 		}
/// 	}
///
/// private:
/// 	std::string text;
/// 	std::optional<jngl::TextInputSession> session;
/// };
/// \endcode
class TextInputSession {
public:
	explicit TextInputSession(TextInputType type = TextInputType::Text);

	TextInputSession(const TextInputSession&) = delete;
	TextInputSession& operator=(const TextInputSession&) = delete;
	TextInputSession(TextInputSession&&) noexcept;
	TextInputSession& operator=(TextInputSession&&) noexcept;
	~TextInputSession();

	/// Returns the UTF-8 characters typed since the last call to take() and clears them
	///
	/// Unlike the deprecated jngl::getTextInput(), characters accumulate across frames, so a
	/// frame in which take() isn't called (e.g. because a modal dialog is open, or the scene is
	/// paused) doesn't lose keystrokes. The internal buffer is capped, so an instance that's
	/// constructed but never read from can't grow unbounded.
	std::string take();

	/// Tells the OS/IME where on screen this field is, in JNGL Screen coordinates (i.e. (0, 0) is
	/// the center of the screen, same as jngl::getMousePos())
	///
	/// This is used to position the IME candidate window (e.g. for CJK input) directly below the
	/// field instead of e.g. the top left corner of the window. \a cursor is the horizontal
	/// offset of the caret within \a area, in the same coordinate system, and defaults to the
	/// start of the field.
	///
	/// Only has an effect while this is the active (i.e. most recently constructed, not yet
	/// destroyed) TextInputSession.
	void setInputArea(Rect area, double cursor = 0);

private:
	struct Impl;
	std::unique_ptr<Impl> impl;

	/// Applies whatever the current top of TextInputSession::stack_ (or the legacy fallback)
	/// wants, by calling into Window. Needed by the internal:: functions below since they aren't
	/// members and don't have an instance to call take()/setInputArea() on.
	static void applyActive();

	/// Sends the active session's input area to Window again without restarting text input, which
	/// would cancel an in-progress IME composition
	static void applyArea();

	static std::vector<Impl*> stack;
	static bool legacyStarted;

	friend void internal::feedTextInput(const std::string&);
	friend void internal::reapplyTextInputSession();
	friend void internal::reapplyTextInputArea();
	friend void internal::ensureLegacyTextInputStarted();
};

} // namespace jngl
