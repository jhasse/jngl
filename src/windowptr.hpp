// Copyright 2009-2017 Jan Niklas Hasse <jhasse@gmail.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "window.hpp"

namespace jngl {
	class WindowPointer {
	public:
		WindowPointer();
		WindowPointer(const WindowPointer&) = delete;
		WindowPointer& operator=(const WindowPointer&) = delete;
		~WindowPointer();
		Window* operator->() const;
		operator bool() const;
		void Delete();
		void Set(Window*);
		void ThrowIfNull() const;
		Window* get() const;
	private:
		Window* ptr_;
	};

	extern WindowPointer pWindow;
}
