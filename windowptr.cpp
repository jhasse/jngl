/*
Copyright 2009 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
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
			throw std::runtime_error("Window hasn't been created yet. Use jngl::ShowWindow.");
		}
	}
}
