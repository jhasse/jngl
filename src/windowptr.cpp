/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#include "windowptr.hpp"

#include <stdexcept>
#include <cassert>

namespace jngl
{
	WindowPointer::WindowPointer() : ptr_(0)
	{
	}

	WindowPointer::~WindowPointer()
	{
		Delete();
	}

	Window* WindowPointer::operator->() const
	{
		ThrowIfNull();
		return ptr_;
	}

	WindowPointer::operator bool() const
	{
		return ptr_ != 0;
	}

	void WindowPointer::Set(Window* ptr)
	{
		assert(ptr_ == 0);
		ptr_ = ptr;
	}

	void WindowPointer::Delete()
	{
		delete ptr_;
		ptr_ = 0;
	}

	void WindowPointer::ThrowIfNull() const
	{
		if(ptr_ == 0)
		{
			throw std::runtime_error("Window hasn't been created yet. Use jngl::showWindow.");
		}
	}

	Window* WindowPointer::get() const {
		return ptr_;
	}
}
