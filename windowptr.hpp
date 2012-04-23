/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "window.hpp"

#include <boost/noncopyable.hpp>

namespace jngl
{
	class WindowPointer : boost::noncopyable {
	public:
		WindowPointer();
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
