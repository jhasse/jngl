// Copyright 2009-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "window.hpp"

namespace jngl {

class WindowPointer {
public:
	WindowPointer();
	WindowPointer(const WindowPointer&) = delete;
	WindowPointer& operator=(const WindowPointer&) = delete;
	WindowPointer(WindowPointer&&) = delete;
	WindowPointer& operator=(WindowPointer&&) = delete;

	/// Leaks the Window if hideWindow() hasn't been called, e.g. because std::exit() was called
	/// while it's open. Destroying it here would tear down its graphics context and the current
	/// Scene in an unspecified order relative to other globals.
	~WindowPointer() = default;
	Window* operator->() const;
	explicit operator bool() const noexcept;
	void Delete();
	void Set(Window*);
	void ThrowIfNull() const;
	Window* get() const noexcept;

private:
	Window* ptr_ = nullptr;
};

extern WindowPointer pWindow;

} // namespace jngl
